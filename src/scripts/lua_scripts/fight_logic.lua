player = gameObjects["fight_player"]
enemy = gameObjects["fight_enemy"]
player_ally = gameObjects["ally"]
enemy_ally = gameObjects["enemy_ally"]
pass_obj = enemy
pass_obj_ally = enemy_ally
player_turn = true
attack_prob = 0.75
enemy_deciding = false
is_shaking = false
shake_count = 6
stop_fight = false

-- main game fight loop logic
function gameLoop()
    if not stop_fight then
        if not player_turn then
            if not enemy_deciding then
                enemy_deciding = true
            end

            -- enemy RNG logic for attacking and defending
            if not is_shaking and enemy_deciding and Delay(0.5) then
                RNG = math.random()
                print("RNG:", RNG)
                if RNG <= attack_prob then
                    print("enemy attack!")
                    EnemyAttack()
                else
                    print("enemy defend!")
                    player_turn = true
                end            
                enemy_deciding = false
            end
        else
            Arrow_Logic()
        end

        -- if enemy lower than 50% health, will defend more often 
        if EnemyDanger() then
            attack_prob = 0.5
        end

        -- enemy dying logic
        if EnemyDead() then
            if enemy.oppacity > 0 then
                if Delay(0.1) then
                    enemy.oppacity = enemy.oppacity - 20
                    enemy_ally.oppacity = enemy_ally.oppacity - 20
                end
            elseif enemy.oppacity <= 0 then
                enemy.oppacity = 0
                enemy_ally.oppacity = 0
                print("YOU WIN!")
                stop_fight = true
            end
        end

        -- player dying logic
        if PlayerDead() then
            if player.oppacity > 0 then
                if Delay(0.1) then
                    player.oppacity = player.oppacity - 20
                    player_ally.oppacity = player_ally.oppacity - 20
                end
            elseif player.oppacity <= 0 then
                player.oppacity = 0
                player_ally.oppacity = 0
                print("YOU LOSE!")
                stop_fight = true
            end
        end

        GameObjectShake(pass_obj, pass_obj_ally)
    end

end

-- return wether player turn (used in arrow.lua)
function PlayerTurn()
    if player_turn then
        return true
    else
        return false
    end
end

-- enemy attack
function SetEnemyTurn()
    player_turn = false
end

-- player attack (used in arrow.lua)
function PlayerAttack()
    pass_obj = enemy
    pass_obj_ally = enemy_ally
    shake_count = 6
    is_shaking = true
end

-- enemy attack
function EnemyAttack()
    pass_obj = player
    pass_obj_ally = player_ally
    shake_count = 6
    is_shaking = true
end

-- "pokemon" shake logic to make it look damaged
function GameObjectShake(obj, obj_ally)
    if is_shaking and shake_count > 0 then
        GoDown(obj.name)
        if Delay(0.03) then
            if shake_count % 2 == 0 then
                obj_ally.x = obj_ally.x + 5
            else
                obj_ally.x = obj_ally.x - 5
            end
            shake_count = shake_count - 1
            if shake_count == 0 then
                SetNewBarLowLevel()
                is_shaking = false
                if obj.name == "fight_player" then
                    player_turn = true
                end
            end
        end
    end
end