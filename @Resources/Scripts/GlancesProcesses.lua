-- GlancesProcesses.lua
-- Parses the JSON body of a Glances /processlist (or /processlist/top/N)
-- response delivered via a sibling WebParser measure, sorts client-side by
-- the chosen field, and exposes the top N rows to Rainmeter meters via
-- inline calls like [&MeasureScript:GetName(1)].
--
-- The Glances API does NOT return fields in alphabetical order — the order
-- depends on the server's Python dict insertion order. So we cannot anchor
-- the regex on first/last field names. Instead we extract balanced top-level
-- JSON objects with a brace-counting scanner and pattern-match within each.
--
-- Script measure options:
--   SourceMeasure : WebParser child measure whose StringValue holds the raw JSON
--   SortField     : "cpu_percent" | "memory_percent" | "rss"
--   TopCount      : number of top rows to keep (default 5)
--   Blacklist     : pipe-separated, case-insensitive process names to skip
--   Debug         : "1" to emit !Log diagnostics every Update (default "1")

-- ---------- logging ----------

local function log(msg, level)
    SKIN:Bang('!Log', msg, level or 'Notice')
end

-- ---------- brace-counting JSON object scanner ----------
-- Returns an array of substrings, each one a top-level {...} object from the
-- input text. Correctly skips braces inside JSON strings (including
-- backslash-escaped quotes).
local function extractObjects(text, maxObjects)
    local objects = {}
    local depth, startIdx = 0, nil
    local inString, escape = false, false
    local n = #text
    for i = 1, n do
        local c = text:byte(i)
        if inString then
            if escape then
                escape = false
            elseif c == 92 then         -- '\'
                escape = true
            elseif c == 34 then         -- '"'
                inString = false
            end
        else
            if c == 34 then             -- '"'
                inString = true
            elseif c == 123 then        -- '{'
                if depth == 0 then startIdx = i end
                depth = depth + 1
            elseif c == 125 then        -- '}'
                depth = depth - 1
                if depth == 0 and startIdx then
                    objects[#objects + 1] = text:sub(startIdx, i)
                    startIdx = nil
                    if maxObjects and #objects >= maxObjects then
                        return objects
                    end
                end
            end
        end
    end
    return objects
end

-- ---------- lifecycle ----------

function Initialize()
    sortField  = SELF:GetOption('SortField', 'cpu_percent')
    topCount   = tonumber(SELF:GetOption('TopCount', '5')) or 5
    sourceName = SELF:GetOption('SourceMeasure', '')
    debugLog   = SELF:GetOption('Debug', '1') == '1'

    blacklist = {}
    local bl = SELF:GetOption('Blacklist', '')
    for word in bl:gmatch('[^|]+') do
        local trimmed = word:match('^%s*(.-)%s*$')
        if trimmed and #trimmed > 0 then
            blacklist[trimmed:lower()] = true
        end
    end

    top = {}
    status = 'init'

    -- Always log on init (regardless of Debug flag) so the user can see
    -- that the script loaded at all.
    log(string.format(
        '[GlancesProcesses] Initialize sort=%s top=%d source=%s blacklist=%d debug=%s',
        sortField, topCount, sourceName,
        (function() local n=0; for _ in pairs(blacklist) do n=n+1 end; return n end)(),
        tostring(debugLog)
    ))
end

function Update()
    if sourceName == '' then
        status = 'no SourceMeasure'
        if debugLog then log('[GlancesProcesses] ' .. status, 'Warning') end
        return 0
    end

    local src = SKIN:GetMeasure(sourceName)
    if not src then
        status = 'source measure missing: ' .. sourceName
        if debugLog then log('[GlancesProcesses] ' .. status, 'Warning') end
        return 0
    end

    local data = src:GetStringValue()
    local len = data and #data or 0
    if len < 2 then
        status = 'empty src (' .. len .. ' bytes)'
        if debugLog then log('[GlancesProcesses] ' .. status, 'Notice') end
        return 0
    end

    local objects = extractObjects(data)

    local processes = {}
    for _, obj in ipairs(objects) do
        local name = obj:match('"name"%s*:%s*"([^"]+)"')
        if name and not blacklist[name:lower()] then
            local cpu  = tonumber(obj:match('"cpu_percent"%s*:%s*([%-0-9%.eE]+)'))    or 0
            local memp = tonumber(obj:match('"memory_percent"%s*:%s*([%-0-9%.eE]+)')) or 0
            -- "rss" only appears inside the per-process memory_info dict.
            local rss  = tonumber(obj:match('"rss"%s*:%s*([0-9]+)')) or 0

            -- "cmdline": ["/path/to/exe", "-arg1", "value1", ...].
            -- This is a flat array of JSON strings (no nested arrays/objects
            -- inside cmdline per Glances schema), so a non-greedy capture
            -- between "[" and "]" is safe. Concatenate the string elements
            -- with single spaces to form a readable command line.
            local cmdline = ''
            local arr = obj:match('"cmdline"%s*:%s*%[(.-)%]')
            if arr then
                local parts = {}
                for s in arr:gmatch('"([^"]*)"') do
                    parts[#parts + 1] = s
                end
                cmdline = table.concat(parts, ' ')
            end

            processes[#processes + 1] = {
                name           = name,
                cpu_percent    = cpu,
                memory_percent = memp,
                rss            = rss,
                cmdline        = cmdline,
            }
        end
    end

    table.sort(processes, function(a, b)
        return (a[sortField] or 0) > (b[sortField] or 0)
    end)

    top = {}
    for i = 1, math.min(topCount, #processes) do
        top[i] = processes[i]
    end

    status = string.format(
        'OK %d/%d procs, json=%dB, top1=%s',
        #top, #processes, len, (top[1] and top[1].name) or '-'
    )

    if debugLog then log('[GlancesProcesses] ' .. status, 'Debug') end

    return #top
end

-- ---------- meter accessors ----------

local function _row(idx)
    idx = tonumber(idx) or 1
    return top[idx]
end

function GetName(idx)
    local p = _row(idx)
    return (p and p.name) or ''
end

function GetCpuPercent(idx)
    local p = _row(idx)
    if not p then return '' end
    return string.format('%.1f', p.cpu_percent)
end

function GetMemoryPercent(idx)
    local p = _row(idx)
    if not p then return '' end
    return string.format('%.1f', p.memory_percent)
end

function GetRss(idx)
    local p = _row(idx)
    return (p and tostring(p.rss)) or '0'
end

function GetCmdline(idx)
    local p = _row(idx)
    return (p and p.cmdline) or ''
end

function GetRssHuman(idx)
    local p = _row(idx)
    if not p then return '' end
    local b = p.rss
    if b >= 1073741824 then
        return string.format('%.1f G', b / 1073741824)
    elseif b >= 1048576 then
        return string.format('%.0f M', b / 1048576)
    elseif b >= 1024 then
        return string.format('%.0f K', b / 1024)
    else
        return string.format('%d B', b)
    end
end

function GetStatus()
    return status or 'no status'
end
