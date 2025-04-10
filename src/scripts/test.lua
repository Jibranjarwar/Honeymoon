player = gameObjects["player"]

stopMovement = false

collided = nil

print("Player name: ", player.name)
print("Pos X: ", player.x)
print("Pos Y: ", player.y)

function gameLoop()

    player:Movement()
    player.children["bob"]:Movement()
    enemyScript()

    -- if player.x > 600 then
    --    player.x = 100
    -- end
end