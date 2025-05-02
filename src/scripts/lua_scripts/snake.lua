snake = gameObjects["snake_head"]
body = gameObjects["snake_body"]
gameOver_obj = gameObjects["gameOver"]
value_x = -10
value_y = 0
body_parts = {body}
prev_positions = {}
body_length = 30
run_game = true

function gameLoop()
    if run_game then
        gameOver_obj.oppacity = 0
        if IsKeyPressed(UP_key) then
            value_y = 10
            value_x = 0
        elseif IsKeyPressed(DOWN_key) then
            value_y = -10
            value_x = 0
        elseif IsKeyPressed(LEFT_key) then
            value_y = 0
            value_x = -10
        elseif IsKeyPressed(RIGHT_key) then
            value_y = 0
            value_x = 10
        end

        -- collision logic
        if snake:OnCollision() then
            local obj = snake:OnCollisionReturn()
            print("collided obj: ", obj.name)
            
            if obj.name == "apple" then
                local body_obj = body:Copy()
                body_obj:AddCollision()
                table.insert(body_parts, body_obj)
            
            elseif obj.name == "snake_body_copy" then
                run_game = false
                copy_gameOver = gameOver_obj:Copy()
                table.insert(body_parts, copy_gameOver)
                copy_gameOver.oppacity = 255

            end
        end

        if Delay(0.05) then
            table.insert(prev_positions, 1, { x = snake.x, y = snake.y })

            -- Move head
            snake.x = snake.x + value_x
            snake.y = snake.y + value_y

            -- Make body parts follow previous positions
            for i, part in ipairs(body_parts) do
                local pos = prev_positions[(i * body_length / 10) + 1]  -- Delay per body
                if pos then
                    part.x = pos.x
                    part.y = pos.y
                end
            end

            -- Trim prev_positions to not grow forever
            local max_length = (#body_parts + 1) * (body_length / 10)
            while #prev_positions > max_length do
                table.remove(prev_positions)
            end
        end
    else
        -- gameOver_obj.oppacity = 255
        if IsKeyPressed(Q_key) then
            snake.x = 240
            snake.y = 50
            body.x = 270
            body.y = 50
            DeleteGameObjects(body_parts)
            body_parts = {body}
            prev_positions = {}
            run_game = true
        end    
    end
end