local frames, named, sent, messages = {}, {}, {}, {}
local unpack = unpack or table.unpack
local methods = {}
local meta = {__index = methods}
local function node() return setmetatable({scripts={},visible=false},meta) end
local function noop() end
for _, name in ipairs({"SetSize","SetBackdrop","SetBackdropColor","SetBackdropBorderColor","SetClampedToScreen","SetMovable","EnableMouse","RegisterForDrag","RegisterEvent","SetMinMaxValues","SetStatusBarTexture","SetAllPoints","SetWidth","SetJustifyH","SetOwner","AddLine"}) do methods[name]=noop end
function methods:SetScript(name,callback) self.scripts[name]=callback end
function methods:GetScript(name) return self.scripts[name] end
function methods:SetPoint(point,relative,relativePoint,x,y) self.point={point,relative,relativePoint,x,y} end
function methods:GetPoint() return unpack(self.point) end
function methods:ClearAllPoints() self.point=nil end
function methods:SetScale(value) self.scale=value end
function methods:StartMoving() self.moving=true end
function methods:StopMovingOrSizing() self.moving=false end
function methods:Hide() self.visible=false end
function methods:Show() self.visible=true end
function methods:SetTexture(value) self.texture=value end
function methods:SetText(value) self.text=value end
function methods:SetValue(value) self.valueSet=value end
function methods:SetStatusBarColor(...) self.color={...} end
function methods:CreateFontString() return node() end
function methods:CreateTexture() return node() end
function CreateFrame(kind,name,parent)
    local f=node();f.parent=parent;frames[#frames+1]=f
    if name then named[name]=f end
    return f
end
function GetLocale() return "frFR" end
function UnitName() return "SurvivalTest" end
function SendAddonMessage(...) sent[#sent+1]={...} end
UIParent=node();GameTooltip=node();SlashCmdList={}
DEFAULT_CHAT_FRAME={AddMessage=function(self,text) messages[#messages+1]=text end}
dofile(arg[1])
local panel,events=named.SylvaniaSurvivalPanel,frames[#frames]
local checks=0
local function check(value,message) checks=checks+1;assert(value,message) end
local function event(...) events.scripts.OnEvent(events,...) end
local function update(seconds) events.scripts.OnUpdate(events,seconds) end
local function receive(data,sender,channel) event("CHAT_MSG_ADDON","SYLSURV",data,channel or "WHISPER",sender or "SurvivalTest") end
local function slash(command) SlashCmdList.SYLVANIASURVIVAL(command) end
event("PLAYER_ENTERING_WORLD")
check(not panel.visible,"ordinary characters start hidden")
update(3)
check(#sent==1 and sent[1][3]=="WHISPER" and sent[1][4]=="SurvivalTest","self-only handshake")
receive("V2;1;500000;600000;700000;1000000;0","OtherPlayer")
receive("V2;1;500000;600000;700000;1000000;0",nil,"GUILD")
check(not panel.visible,"other senders and channels cannot activate HUD")
receive("V1;1;0;60;70")
check(panel.visible and panel.bars[1].valueSet==0,"legacy gauges remain compatible")
check(not panel.penalties[1].visible,"rounded legacy zero never invents a hunger penalty")
receive("V2;1;7580;0;800000;10000;6")
check(panel.bars[1].valueSet==0.758 and panel.bars[2].valueSet==0,"raw values retain precision")
check(not panel.penalties[1].visible and panel.penalties[2].visible and panel.penalties[3].visible,"only actual server flags display penalties")
receive("V2;1;0;0;800000;10000;7")
check(panel.penalties[1].visible,"hunger badge follows server confirmation")
panel.penalties[1].scripts.OnEnter(panel.penalties[1])
check(GameTooltip.visible and GameTooltip.text=="Affamé","specific hunger tooltip is available")
receive("V2;1;100000;200000;0;1000000;0")
check(not panel.penalties[1].visible and not panel.penalties[2].visible and not panel.penalties[3].visible,"recovery clears all badges")
local previous=panel.bars[1].valueSet
for _,invalid in ipairs({"V2;1;-1;0;0;10000;0","V2;1;1000001;0;0;10000;0","V2;1;0;0;0;9999;0","V2;1;0;0;0;10000;8","V2;1;0;0;0;10000;1.5","V2;1;0;0;0;10000;0;extra","V1;1;0.5;0;0","V3;1;0;0;0"}) do receive(invalid) end
check(panel.bars[1].valueSet==previous,"invalid protocol data cannot replace current state")
receive("V2;0;0;0;0;10000;0")
check(not panel.visible,"pause and completion hide HUD")
slash("test")
check(panel.visible and panel.penalties[1].visible and panel.penaltyText.text:find("Affichage uniquement"),"explicit preview is clearly labelled")
receive("V2;0;1000000;1000000;0;1000000;0")
check(panel.penalties[1].visible,"inactive replies do not overwrite preview")
slash("stop")
check(not panel.visible,"preview can be dismissed")
slash("test");update(61)
check(not panel.visible,"preview expires without changing server state")
slash("scale 120")
check(panel.scale==1.2 and SylvaniaSurvivalDB.scale==1.2,"scale is saved per character")
slash("scale 999")
check(panel.scale==1.6,"scale has a screen-safe upper bound")
slash("unlock");panel.scripts.OnDragStart(panel)
check(panel.moving,"unlocked panel can be moved")
panel:SetPoint("CENTER",UIParent,"CENTER",100,-25);panel.scripts.OnDragStop(panel)
check(SylvaniaSurvivalDB.position[1]=="CENTER" and SylvaniaSurvivalDB.position[2]==100,"drag position is saved")
slash("lock");panel.scripts.OnDragStart(panel)
check(not panel.moving,"locked panel stays in position")
slash("reset")
check(panel.scale==1 and panel.point[1]=="TOPRIGHT" and not SylvaniaSurvivalDB.locked,"reset restores a visible default position")
receive("V2;1;1000000;1000000;0;1000000;0")
local calls=#sent;slash("test")
check(panel.bars[1].valueSet==100 and #sent==calls,"preview never masks real participant data")
update(21)
check(not panel.visible,"stale live data disappears")
receive("V2;1;1;1;0;1000000;0");event("PLAYER_ENTERING_WORLD")
check(not panel.visible,"realm and character changes clear live state")
check(panel.bars[1].label.text=="Nourriture","French food label is explicit")
check(panel.penalties[2].icon.texture=="Interface\\AddOns\\SylvaniaSurvival\\Textures\\Water","custom waterskin asset is wired")
print(checks.." addon checks passed")
