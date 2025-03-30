player = gameObjects[3]

stopMovement = false

print("Player name: ", player.name)
print("Pos X: ", player.x)
print("Pos Y: ", player.y)

function gameLoop()
    
    if not stopMovement then
        player:Movement()
    end

    if player:OnCollision() then
        print("we collided")
        player.x = 100
        player.y = 300
        stopMovement = true
    else
        print("we have not collided with anything")
    end
end