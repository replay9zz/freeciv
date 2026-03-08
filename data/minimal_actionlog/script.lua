-- Freeciv - Copyright (C) 2007 - The Freeciv Project
--   This program is free software; you can redistribute it and/or modify
--   it under the terms of the GNU General Public License as published by
--   the Free Software Foundation; either version 2, or (at your option)
--   any later version.
--
--   This program is distributed in the hope that it will be useful,
--   but WITHOUT ANY WARRANTY; without even the implied warranty of
--   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--   GNU General Public License for more details.

-- This file is for lua-functionality that is specific to a given
-- ruleset. When freeciv loads a ruleset, it also loads script
-- file called 'default.lua'. The one loaded if your ruleset
-- does not provide an override is default/default.lua.

-- Custom elimination: resolve based on cities or city-builder units only.
local function count_cities(player)
  if not player then
    return 0
  end
  local iter = player.cities_iterate and player:cities_iterate() or nil
  if iter then
    local count = 0
    while true do
      local city = iter()
      if not city then
        break
      end
      count = count + 1
    end
    return count
  end
  if player.num_cities then
    local ok, value = pcall(function() return player:num_cities() end)
    if ok and value then
      return value
    end
  end
  return 0
end

local function has_citybuilder_units(player)
  if not player then
    return false
  end
  local iter = player.units_iterate and player:units_iterate() or nil
  if not iter then
    return true
  end
  while true do
    local unit = iter()
    if not unit then
      break
    end
    local utype = unit.utype
    if not utype or not utype.has_flag then
      return true
    end
    if utype:has_flag("Cities") then
      return true
    end
  end
  return false
end

function check_elimination_victory(...)
  -- If a winner already exists, do nothing.
  for pl in players_iterate() do
    if pl.is_winner and pl:is_winner() then
      return false
    end
  end

  local alive = {}
  for pl in players_iterate() do
    if pl then
      alive[#alive + 1] = pl
    end
  end
  if #alive < 2 then
    return false
  end

  local active = {}
  local eliminated = {}
  for _, pl in ipairs(alive) do
    local cities = count_cities(pl)
    local builders = has_citybuilder_units(pl)
    if cities > 0 or builders then
      active[#active + 1] = pl
    else
      eliminated[#eliminated + 1] = pl
    end
  end

  if #eliminated == 0 then
    return false
  end

  if #active == 1 then
    active[1]:victory()
  elseif #active == 0 then
    for _, pl in ipairs(alive) do
      pl:victory()
    end
  end
  return false
end

signal.connect("city_destroyed", "check_elimination_victory")
signal.connect("city_transferred", "check_elimination_victory")
signal.connect("unit_lost", "check_elimination_victory")
signal.connect("player_alive_phase_end", "check_elimination_victory")
-- Emit structured action logs for offline dataset collection.
local actionlog_prefix = "ACTLOG"

local function actionlog_number(value)
  if value == math.floor(value) then
    return string.format("%.0f", value)
  end

  return tostring(value)
end

local function actionlog_append(parts, key, value)
  if value == nil then
    parts[#parts + 1] = key .. "=null"
  elseif type(value) == "string" then
    parts[#parts + 1] = key .. "=" .. string.format("%q", value)
  elseif type(value) == "boolean" then
    parts[#parts + 1] = key .. "=" .. (value and "1" or "0")
  elseif type(value) == "number" then
    parts[#parts + 1] = key .. "=" .. actionlog_number(value)
  else
    parts[#parts + 1] = key .. "=" .. tostring(value)
  end
end

local function actionlog_add_tile(parts, prefix, tile)
  if tile == nil then
    actionlog_append(parts, prefix .. "_tile", nil)
    return
  end

  actionlog_append(parts, prefix .. "_tile", tile.id)
  actionlog_append(parts, prefix .. "_x", tile.nat_x)
  actionlog_append(parts, prefix .. "_y", tile.nat_y)
  actionlog_append(parts, prefix .. "_terrain", tile.terrain:rule_name())

  if tile.owner ~= nil then
    actionlog_append(parts, prefix .. "_tile_owner", tile.owner.id)
  end

  local city = tile:city()
  if city ~= nil then
    actionlog_append(parts, prefix .. "_city", city.id)
    actionlog_append(parts, prefix .. "_city_name", city.name)
  end
end

local function actionlog_add_player(parts, prefix, player)
  if player == nil then
    actionlog_append(parts, prefix .. "_player", nil)
    return
  end

  actionlog_append(parts, prefix .. "_player", player.id)
  actionlog_append(parts, prefix .. "_name", player.name)
  if player.nation ~= nil then
    actionlog_append(parts, prefix .. "_nation", player.nation:name_translation())
  end
  if player.government ~= nil then
    actionlog_append(parts, prefix .. "_government", player.government:name_translation())
  end
  actionlog_append(parts, prefix .. "_is_alive", player.is_alive)
  actionlog_append(parts, prefix .. "_is_human", player:is_human())
end

local function actionlog_add_unit(parts, prefix, unit)
  if unit == nil then
    actionlog_append(parts, prefix .. "_unit", nil)
    return
  end

  actionlog_append(parts, prefix .. "_unit", unit.id)
  actionlog_append(parts, prefix .. "_type", unit.utype:rule_name())
  actionlog_append(parts, prefix .. "_veteran", unit.veteran)
  actionlog_add_player(parts, prefix, unit.owner)
  actionlog_add_tile(parts, prefix, unit.tile)
end

local function actionlog_add_city(parts, prefix, city)
  if city == nil then
    actionlog_append(parts, prefix .. "_city", nil)
    return
  end

  actionlog_append(parts, prefix .. "_city", city.id)
  actionlog_append(parts, prefix .. "_name", city.name)
  actionlog_append(parts, prefix .. "_size", city.size)
  actionlog_add_player(parts, prefix, city.owner)
  actionlog_add_tile(parts, prefix, city.tile)
end

local function actionlog_emit(event_name, builder)
  local parts = {
    actionlog_prefix,
    "event=" .. event_name,
    "turn=" .. actionlog_number(game.current_turn()),
    "year=" .. actionlog_number(game.current_year()),
    "year_text=" .. string.format("%q", game.current_year_text()),
    "ruleset=" .. string.format("%q", game.rulesetdir()),
  }

  if builder ~= nil then
    builder(parts)
  end

  log.normal(table.concat(parts, "\t"))
end

function actionlog_turn_begin_callback(turn, year)
  actionlog_emit("turn_begin", function(parts)
    actionlog_append(parts, "signal_turn", turn)
    actionlog_append(parts, "signal_year", year)
  end)
  return false
end

function actionlog_player_phase_begin_callback(player, is_alive)
  actionlog_emit("player_phase_begin", function(parts)
    actionlog_add_player(parts, "phase", player)
    actionlog_append(parts, "phase_signal_alive", is_alive)
  end)
  return false
end

function actionlog_player_phase_end_callback(player)
  actionlog_emit("player_phase_end", function(parts)
    actionlog_add_player(parts, "phase", player)
  end)
  return false
end

function actionlog_unit_moved_callback(unit, from_tile, to_tile)
  actionlog_emit("unit_moved", function(parts)
    actionlog_add_unit(parts, "actor", unit)
    actionlog_add_tile(parts, "from", from_tile)
    actionlog_add_tile(parts, "to", to_tile)
  end)
  return false
end

function actionlog_city_built_callback(city)
  actionlog_emit("city_built", function(parts)
    actionlog_add_city(parts, "target", city)
  end)
  return false
end

function actionlog_unit_built_callback(unit, city)
  actionlog_emit("unit_built", function(parts)
    actionlog_add_unit(parts, "actor", unit)
    actionlog_add_city(parts, "source", city)
  end)
  return false
end

function actionlog_building_built_callback(building, city)
  actionlog_emit("building_built", function(parts)
    actionlog_append(parts, "building", building:rule_name())
    actionlog_add_city(parts, "target", city)
  end)
  return false
end

function actionlog_tech_researched_callback(tech, player, how)
  actionlog_emit("tech_researched", function(parts)
    actionlog_append(parts, "tech", tech:rule_name())
    actionlog_append(parts, "how", how)
    actionlog_add_player(parts, "actor", player)
  end)
  return false
end

function actionlog_action_finished_unit_unit_callback(action, success, actor, target)
  actionlog_emit("action_finished_unit_unit", function(parts)
    actionlog_append(parts, "action", action:rule_name())
    actionlog_append(parts, "success", success)
    actionlog_add_unit(parts, "actor", actor)
    actionlog_add_unit(parts, "target", target)
  end)
  return false
end

function actionlog_action_finished_unit_units_callback(action, success, actor, target)
  actionlog_emit("action_finished_unit_units", function(parts)
    actionlog_append(parts, "action", action:rule_name())
    actionlog_append(parts, "success", success)
    actionlog_add_unit(parts, "actor", actor)
    actionlog_add_tile(parts, "target", target)
  end)
  return false
end

function actionlog_action_finished_unit_city_callback(action, success, actor, target)
  actionlog_emit("action_finished_unit_city", function(parts)
    actionlog_append(parts, "action", action:rule_name())
    actionlog_append(parts, "success", success)
    actionlog_add_unit(parts, "actor", actor)
    actionlog_add_city(parts, "target", target)
  end)
  return false
end

function actionlog_action_finished_unit_tile_callback(action, success, actor, target)
  actionlog_emit("action_finished_unit_tile", function(parts)
    actionlog_append(parts, "action", action:rule_name())
    actionlog_append(parts, "success", success)
    actionlog_add_unit(parts, "actor", actor)
    actionlog_add_tile(parts, "target", target)
  end)
  return false
end

function actionlog_action_finished_unit_extras_callback(action, success, actor, target)
  actionlog_emit("action_finished_unit_extras", function(parts)
    actionlog_append(parts, "action", action:rule_name())
    actionlog_append(parts, "success", success)
    actionlog_add_unit(parts, "actor", actor)
    actionlog_add_tile(parts, "target", target)
  end)
  return false
end

function actionlog_action_finished_unit_self_callback(action, success, actor)
  actionlog_emit("action_finished_unit_self", function(parts)
    actionlog_append(parts, "action", action:rule_name())
    actionlog_append(parts, "success", success)
    actionlog_add_unit(parts, "actor", actor)
  end)
  return false
end

signal.connect("turn_begin", "actionlog_turn_begin_callback")
signal.connect("player_phase_begin", "actionlog_player_phase_begin_callback")
signal.connect("player_phase_end", "actionlog_player_phase_end_callback")
signal.connect("unit_moved", "actionlog_unit_moved_callback")
signal.connect("city_built", "actionlog_city_built_callback")
signal.connect("unit_built", "actionlog_unit_built_callback")
signal.connect("building_built", "actionlog_building_built_callback")
signal.connect("tech_researched", "actionlog_tech_researched_callback")
signal.connect("action_finished_unit_unit", "actionlog_action_finished_unit_unit_callback")
signal.connect("action_finished_unit_units", "actionlog_action_finished_unit_units_callback")
signal.connect("action_finished_unit_city", "actionlog_action_finished_unit_city_callback")
signal.connect("action_finished_unit_tile", "actionlog_action_finished_unit_tile_callback")
signal.connect("action_finished_unit_extras", "actionlog_action_finished_unit_extras_callback")
signal.connect("action_finished_unit_self", "actionlog_action_finished_unit_self_callback")
