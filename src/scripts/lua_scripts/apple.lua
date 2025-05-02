apple = gameObjects["apple"]

function gameLoop()

    if apple:OnCollision() then
        print("pressed button")
        local obj = apple:OnCollisionReturn()
        print(type(obj))
        print(obj.name)
        if obj.name == "snake_head" or obj.name == "snake_body_copy" or obj.name == "snake_body" then
            local x_position = math.random(40, 512)
            local y_position = math.random(-240, 230)
            apple.x = x_position
            apple.y = y_position
        end
    end

end