--[[
  Client-side vision helpers for LuaRemote agents.

  Provides:
    list_visible_tiles(player_id, unit_id[, vlayer]) -> string
    unit_vision_radius(player_id, unit_id) -> radius_sq, radius

  Coordinates are returned in native (map) tile coordinates.
]]

local V_LAYER_MAIN = 0
local MAX_VLAYER = 2

local SCRIPT_PATH = '/lua/unit_vision.lua'

local function trace(fn_name)
  log.normal('Execute %s:%s', SCRIPT_PATH, fn_name)
end

local function resolve_unit(player_id, unit_id)
  local player = find.player(player_id)
  if not player then
    error(string.format('unknown player id %s', tostring(player_id)))
  end

  local unit = find.unit(player, unit_id)
  if not unit then
    error(string.format('unknown unit id %s for player %s', tostring(unit_id), tostring(player_id)))
  end

  return unit
end

local function clamp_layer(layer)
  if layer == nil then
    return V_LAYER_MAIN
  end
  if type(layer) ~= 'number' or layer < 0 or layer > MAX_VLAYER then
    error(string.format('invalid vision layer %s', tostring(layer)))
  end
  return layer
end

function unit_vision_radius(player_id, unit_id)
  trace('unit_vision_radius')
  local unit = resolve_unit(player_id, unit_id)
  local radius_sq = client.unit_vision_radius_sq(unit.id)
  if radius_sq < 0 then
    return -1, -1
  end
  return radius_sq, math.sqrt(radius_sq)
end

function list_visible_tiles(player_id, unit_id, layer)
  trace('list_visible_tiles')
  local unit = resolve_unit(player_id, unit_id)
  local host_tile = unit.tile
  if not host_tile then
    return ''
  end

  local vlayer = clamp_layer(layer)
  local radius_sq = client.unit_vision_radius_sq(unit.id)
  if radius_sq < 0 then
    return ''
  end

  local tiles = {}

  for tile in host_tile:circle_iterate(radius_sq) do
    if tile and client.tile_seen(tile.id, vlayer) then
      tiles[#tiles + 1] = string.format('%d,%d', tile.nat_x, tile.nat_y)
    end
  end

  return table.concat(tiles, ';')
end

return {
  list_visible_tiles = list_visible_tiles,
  unit_vision_radius = unit_vision_radius,
}
