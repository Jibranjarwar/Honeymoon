enemy_bar = gameObjects["enemy_health_bar"].children["enemy_bar"]
ally_bar = gameObjects["player_health_bar"].children["player_bar"]
enemy_bar_next = enemy_bar.width - 10
ally_bar_next = ally_bar.width - 10
original_enemy_health = enemy_bar.width

-- healthbar decreasing logic
function GoDown(name)
    if name == "fight_enemy" then
        if enemy_bar.width > enemy_bar_next then
            enemy_bar.width = enemy_bar.width - 1
        end
    elseif name == "fight_player" then
        if ally_bar.width > ally_bar_next then
            ally_bar.width = ally_bar.width - 1
        end
    end
end

-- set newbar minimum
function SetNewBarLowLevel()
    enemy_bar_next = enemy_bar.width - 10
    ally_bar_next = ally_bar.width - 10
end

-- on dying enemy function
function EnemyDead()
    if enemy_bar.width <= 0 then
        return true
    end
    return false
end

-- on dying player function
function PlayerDead()
    if ally_bar.width <= 0 then
        return true
    end
    return false
end

-- function for enemy RNG change
function EnemyDanger()
    if enemy_bar.width < (original_enemy_health // 2) then
        return true
    else
        return false
    end
end