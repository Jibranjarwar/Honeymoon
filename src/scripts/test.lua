player = gameObjects[3]

print("Player name: ", player.name)
print("Pos X: ", player.x)
print("Pos Y: ", player.y)

function gameLoop()
    
    player:Movement()
    
    if player.x > 500 then
        player.x = 200
    else
        print("This is the else statement")
    end
    -- player:UpdatePosX(50)
    print("New Pos X: ", player.x)
end