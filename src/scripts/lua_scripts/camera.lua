camera_obj = Camera
player = gameObjects["player"]

function CameraFunctionality()
    camera_obj:FollowObject(player)
end

function CameraSwitch()
    camera_obj.x = 910
    camera_obj.y = 50
    camera_obj.width = 400
    camera_obj.height = 400
end