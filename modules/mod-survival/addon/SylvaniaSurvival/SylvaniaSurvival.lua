local french = GetLocale() == "frFR"
local function L(fr, en) return french and fr or en end
local labels = {L("Nourriture", "Food"), L("Hydratation", "Hydration"), L("Froid", "Cold")}
local icons = {"Food", "Water", "Cold"}
local texturePath = "Interface\\AddOns\\SylvaniaSurvival\\Textures\\"
local backdrop = {bgFile = "Interface\\Tooltips\\UI-Tooltip-Background", edgeFile = "Interface\\Tooltips\\UI-Tooltip-Border", tile = true, tileSize = 16, edgeSize = 12, insets = {left = 3, right = 3, top = 3, bottom = 3}}
local state, preview, received = nil, false, false
local sinceHello, sinceUpdate, previewTime = 8, 0, 0

local panel = CreateFrame("Frame", "SylvaniaSurvivalPanel", UIParent)
panel:SetSize(290, 224)
panel:SetPoint("TOPRIGHT", UIParent, "TOPRIGHT", -25, -200)
panel:SetBackdrop(backdrop)
panel:SetBackdropColor(0.035, 0.027, 0.018, 0.94)
panel:SetBackdropBorderColor(0.65, 0.49, 0.25, 1)
panel:SetClampedToScreen(true)
panel:SetMovable(true)
panel:EnableMouse(true)
panel:RegisterForDrag("LeftButton")
panel:Hide()
local heading = panel:CreateFontString(nil, "OVERLAY", "GameFontNormal")
heading:SetPoint("TOPLEFT", 16, -13)
heading:SetText("SYLVANIA |cffe8d3a2SURVIVAL|r")
local status = panel:CreateFontString(nil, "OVERLAY", "GameFontHighlightSmall")
status:SetPoint("TOPRIGHT", -16, -15)
local footer = panel:CreateFontString(nil, "OVERLAY", "GameFontDisableSmall")
footer:SetPoint("BOTTOM", 0, 10)
footer:SetText("/survivalui")

local function tooltip(owner, title, lines)
    GameTooltip:SetOwner(owner, "ANCHOR_LEFT")
    GameTooltip:SetText(title, 1, 0.82, 0.35)
    for _, line in ipairs(lines) do GameTooltip:AddLine(line, 0.9, 0.9, 0.9, true) end
    GameTooltip:Show()
end
local function ruleLines(index)
    if index == 1 then
        local rule = state and state.version == "V2"
            and L("À 0 : Force, Agilité, Endurance, Intelligence et Esprit réduits de 50 %.", "At 0: Strength, Agility, Stamina, Intellect and Spirit reduced by 50%.")
            or L("Première version serveur : une réserve vide abaisse la vie jusqu’à 1 %.", "First server version: an empty reserve lowers health towards 1%.")
        return {L("Mangez un aliment et laissez son effet agir pour remplir cette réserve.", "Eat food and let its effect run to refill this reserve."), rule}
    elseif index == 2 then
        local lines = {L("Buvez pour remplir cette réserve. La chaleur extérieure accélère la soif.", "Drink to refill this reserve. Outdoor heat increases thirst."), L("À 0 : la vie diminue progressivement jusqu’à 1 %. Boire lève le plafond, sans soigner gratuitement.", "At 0: health gradually drops towards 1%. Drinking lifts the cap without granting free healing.")}
        if state and state.version == "V2" and state.ceiling < 1000000 then
            lines[#lines + 1] = string.format(L("Plafond de vie actuel : %.1f %%.", "Current health cap: %.1f%%."), state.ceiling / 10000)
        end
        return lines
    end
    return {L("Cette barre monte avec l’exposition au froid. L’eau accélère le refroidissement.", "This bar rises with cold exposure. Water accelerates cooling."), L("Dès 75 % : Transi ralentit les déplacements et les attaques de mêlée. À 100 % : légère perte de vie.", "From 75%: Chilled slows movement and melee attacks. At 100%: slight health loss."), L("Cherchez un abri, une auberge ou un feu de cuisine à moins de 8 mètres.", "Find shelter, an inn or a cooking fire within 8 yards.")}
end

local bars, penalties = {}, {}
panel.bars, panel.penalties = bars, penalties
for i = 1, 3 do
    local index = i
    local row = CreateFrame("Frame", nil, panel)
    row:SetSize(258, 34)
    row:SetPoint("TOPLEFT", 16, -38 - (i - 1) * 38)
    row:EnableMouse(true)
    local icon = row:CreateTexture(nil, "ARTWORK")
    icon:SetSize(32, 32)
    icon:SetPoint("LEFT", 0, 0)
    icon:SetTexture(texturePath .. icons[i])
    local bar = CreateFrame("StatusBar", nil, row)
    bar:SetSize(218, 22)
    bar:SetPoint("LEFT", 40, 0)
    bar:SetMinMaxValues(0, 100)
    bar:SetStatusBarTexture("Interface\\TargetingFrame\\UI-StatusBar")
    local background = bar:CreateTexture(nil, "BACKGROUND")
    background:SetAllPoints()
    background:SetTexture(0.10, 0.09, 0.075, 1)
    bar.label = bar:CreateFontString(nil, "OVERLAY", "GameFontHighlightSmall")
    bar.label:SetPoint("LEFT", 6, 0)
    bar.label:SetText(labels[i])
    bar.value = bar:CreateFontString(nil, "OVERLAY", "GameFontHighlightSmall")
    bar.value:SetPoint("RIGHT", -5, 0)
    row:SetScript("OnEnter", function(self) tooltip(self, labels[index], ruleLines(index)) end)
    row:SetScript("OnLeave", function() GameTooltip:Hide() end)
    row:RegisterForDrag("LeftButton")
    row:SetScript("OnDragStart", function() if not SylvaniaSurvivalDB.locked then panel:StartMoving() end end)
    row:SetScript("OnDragStop", function() panel:GetScript("OnDragStop")(panel) end)
    bars[i] = bar
    local badge = CreateFrame("Frame", nil, panel)
    badge:SetSize(30, 30)
    badge:SetPoint("TOPLEFT", 16 + (i - 1) * 34, -165)
    badge:SetBackdrop(backdrop)
    badge:SetBackdropBorderColor(0.9, 0.15, 0.08, 1)
    badge.icon = badge:CreateTexture(nil, "ARTWORK")
    badge.icon:SetPoint("TOPLEFT", 3, -3)
    badge.icon:SetPoint("BOTTOMRIGHT", -3, 3)
    badge.icon:SetTexture(texturePath .. icons[i])
    badge:EnableMouse(true)
    badge:SetScript("OnEnter", function(self)
        tooltip(self, ({L("Affamé", "Starving"), L("Déshydraté", "Dehydrated"), L("Transi", "Chilled")})[index], ruleLines(index))
    end)
    badge:SetScript("OnLeave", function() GameTooltip:Hide() end)
    badge:Hide()
    penalties[i] = badge
end
local penaltyText = panel:CreateFontString(nil, "OVERLAY", "GameFontHighlightSmall")
penaltyText:SetPoint("TOPLEFT", 122, -169)
penaltyText:SetWidth(150)
penaltyText:SetJustifyH("LEFT")
panel.penaltyText = penaltyText

local function settings()
    if type(SylvaniaSurvivalDB) ~= "table" then SylvaniaSurvivalDB = {} end
    local db = SylvaniaSurvivalDB
    db.scale = math.max(0.8, math.min(1.6, tonumber(db.scale) or 1))
    panel:SetScale(db.scale)
    local allowed = {TOPLEFT=true,TOP=true,TOPRIGHT=true,LEFT=true,CENTER=true,RIGHT=true,BOTTOMLEFT=true,BOTTOM=true,BOTTOMRIGHT=true}
    if type(db.position) == "table" and allowed[db.position[1]] and tonumber(db.position[2]) and tonumber(db.position[3]) then
        panel:ClearAllPoints()
        panel:SetPoint(db.position[1], UIParent, db.position[1], db.position[2], db.position[3])
    end
end
panel:SetScript("OnDragStart", function(self) if not SylvaniaSurvivalDB.locked then self:StartMoving() end end)
panel:SetScript("OnDragStop", function(self)
    self:StopMovingOrSizing()
    local point, _, _, x, y = self:GetPoint()
    SylvaniaSurvivalDB.position = {point, x, y}
end)
local function render()
    if not state or (not received and not preview) then panel:Hide(); return end
    status:SetText(preview and "|cffffa94dTEST|r" or "|cff8cc977R1|r")
    for i = 1, 3 do
        local value = state.values[i]
        bars[i]:SetValue(value)
        bars[i].value:SetText(string.format("%.1f %%", value))
        if (i == 3 and value >= 75) or (i ~= 3 and value <= 10) then
            bars[i]:SetStatusBarColor(0.78, 0.17, 0.12)
        elseif (i == 3 and value >= 50) or (i ~= 3 and value <= 25) then
            bars[i]:SetStatusBarColor(0.85, 0.52, 0.14)
        elseif i == 2 then
            bars[i]:SetStatusBarColor(0.16, 0.46, 0.72)
        elseif i == 3 then
            bars[i]:SetStatusBarColor(0.36, 0.65, 0.76)
        else
            bars[i]:SetStatusBarColor(0.34, 0.54, 0.20)
        end
        local active = math.floor(state.flags / (2 ^ (i - 1))) % 2 == 1
        if active then penalties[i]:Show() else penalties[i]:Hide() end
    end
    if preview then
        penaltyText:SetText(L("Aperçu des malus\nAffichage uniquement", "Penalty preview\nDisplay only"))
    elseif state.version == "V1" then
        penaltyText:SetText(L("Jauges actives\nAncienne version serveur", "Live meters\nOlder server version"))
    elseif state.flags == 0 then
        penaltyText:SetText(L("Aucun malus\nSurvival actif", "No penalties\nSurvival active"))
    else
        penaltyText:SetText(L("Malus actifs\nSurvolez les icônes", "Active penalties\nHover over icons"))
    end
    panel:Show()
end
local function parse(payload)
    if type(payload) ~= "string" or #payload > 180 then return end
    local fields = {}
    for field in (payload .. ";"):gmatch("(.-);") do fields[#fields + 1] = field end
    local version, active = fields[1], fields[2]
    if active ~= "0" and active ~= "1" then return end
    local maximum = version == "V1" and 100 or 1000000
    if not ((version == "V1" and #fields == 5) or (version == "V2" and #fields == 7)) then return end
    local values = {}
    for i = 1, 3 do
        local value = tonumber(fields[i + 2])
        if not value or value ~= math.floor(value) or value < 0 or value > maximum then return end
        values[i] = value * 100 / maximum
    end
    local ceiling, flags = 1000000, 0
    if version == "V2" then
        ceiling, flags = tonumber(fields[6]), tonumber(fields[7])
        if not ceiling or ceiling ~= math.floor(ceiling) or ceiling < 10000 or ceiling > 1000000 then return end
        if not flags or flags ~= math.floor(flags) or flags < 0 or flags > 7 then return end
    end
    return {version=version, active=active == "1", values=values, ceiling=ceiling, flags=flags}
end
local function hello()
    if UnitName("player") then SendAddonMessage("SYLSURV", "HELLO", "WHISPER", UnitName("player")) end
    sinceHello = 0
end
local events = CreateFrame("Frame")
events:RegisterEvent("PLAYER_ENTERING_WORLD")
events:RegisterEvent("PLAYER_LOGOUT")
events:RegisterEvent("CHAT_MSG_ADDON")
events:SetScript("OnEvent", function(self, event, prefix, payload, channel, sender)
    if event == "PLAYER_ENTERING_WORLD" or event == "PLAYER_LOGOUT" then
        settings()
        received, preview, state = false, false, nil
        sinceHello, sinceUpdate = 8, 0
        panel:Hide()
        return
    end
    if prefix ~= "SYLSURV" or channel ~= "WHISPER" or sender ~= UnitName("player") then return end
    local incoming = parse(payload)
    if not incoming then return end
    if preview and not incoming.active then return end
    state, received, sinceUpdate = incoming, incoming.active, 0
    if received then preview = false end
    render()
end)
events:SetScript("OnUpdate", function(self, elapsed)
    sinceHello, sinceUpdate = sinceHello + elapsed, sinceUpdate + elapsed
    if received and sinceUpdate > 20 then received = false; state = nil; render() end
    if preview then
        previewTime = previewTime - elapsed
        if previewTime <= 0 then preview = false; state = nil; render() end
    end
    if sinceHello >= 10 then hello() end
end)
SLASH_SYLVANIASURVIVAL1 = "/survivalui"
SlashCmdList.SYLVANIASURVIVAL = function(message)
    settings()
    local command, argument = (message or ""):lower():match("^%s*(%S*)%s*(.-)%s*$")
    if command == "test" or command == "preview" then
        if received then
            DEFAULT_CHAT_FRAME:AddMessage(L("Survival : les jauges réelles sont déjà actives.", "Survival: live meters are already active."))
            return
        end
        preview, previewTime = true, 60
        state = {version="V2",values={0,0,80},ceiling=10000,flags=7}
        render()
    elseif command == "stop" then
        if preview then preview = false; state = nil; render() end
    elseif command == "reset" then
        SylvaniaSurvivalDB = {}
        panel:ClearAllPoints()
        panel:SetPoint("TOPRIGHT", UIParent, "TOPRIGHT", -25, -200)
        settings()
    elseif command == "lock" or command == "unlock" then
        SylvaniaSurvivalDB.locked = command == "lock"
    elseif command == "scale" and tonumber(argument) then
        SylvaniaSurvivalDB.scale = math.max(80, math.min(160, tonumber(argument))) / 100
        settings()
    else
        DEFAULT_CHAT_FRAME:AddMessage(L("Survival : /survivalui test (aperçu 60 s), stop, reset, lock, unlock, scale 80–160. Déplacez le cadre avec la souris.", "Survival: /survivalui test (60 s preview), stop, reset, lock, unlock, scale 80–160. Drag to move the panel."))
        hello()
    end
end
