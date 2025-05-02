arrow = gameObjects["arrow"]
button = {"attack", "defend"}
button_value = button[1]
player_attack = 0

function Arrow_Logic()
    
    if PlayerTurn() then
        if IsKeyPressed(DOWN_key) then
            if not (arrow.y == -295) then
                arrow.y = -295
                button_value = button[2]
            end
        elseif IsKeyPressed(UP_key) then
            if not (arrow.y == -259) then
                arrow.y = -259
                button_value = button[1]
            end
        end
         
        if IsKeyPressed(SPACE_key) then
            SetEnemyTurn()
            if button_value == button[1] then
                player_attack = math.random(10, 30)
                PlayerAttack(player_attack, 1)
                print("player attack!")
            else
                print("player defend!")
            end
        end
    end   
end