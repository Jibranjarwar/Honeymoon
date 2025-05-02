enemy = gameObjects["enemy"]
todelete = {}

function gameLoop()
    -- print("Hello World!")
    if IsKeyHeld(KeycodeToScancode(A_key)) then
        enemy.x = enemy.x - 5
    end
    if IsKeyHeld(KeycodeToScancode(D_key)) then
        enemy.x = enemy.x + 5
    end
    if IsKeyPressed(C_key) then
        obj = enemy:Copy();
        -- table.insert(todelete, obj);
        print("inserted obj:", obj.name)
        print("type", type(todelete))
        table.insert(todelete, obj)
    end

    if IsKeyPressed(Q_key) then
        print("delete")
        print(todelete[1].name)
        DeleteGameObjects(todelete)
        todelete = {}
    --    enemy.x = 100
    --    enemy.y = 0
    end
    -- enemy:Movement()
end

print("exit gameLoop")