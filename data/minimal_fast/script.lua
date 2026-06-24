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
