player = gameObjects["player"]
enemy = gameObjects["enemy"]

function gameLoop()
    player:Movement()
    if player:OnCollision() then
        collided = player:OnCollisionReturn()
        print("name: ", collided.name)
        if collided.name == "enemy" then
            CameraSwitch()
        end
    else
        CameraFunctionality()
    end
end