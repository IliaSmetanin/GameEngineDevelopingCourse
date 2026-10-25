local ecs = require "ecs"
-- local u = require "util"

local function rand_flt(from, to)
	local maxNumber = 32767
    return from + (math.random(maxNumber) / maxNumber) * (to - from)
end

local function move(it)
    for pos, vel, ent in ecs.each(it) do
        pos.x = pos.x + vel.x * it.delta_time
        pos.y = pos.y + vel.y * it.delta_time
		pos.z = pos.z + vel.z * it.delta_time
    end
end

local function gravity(it)
    for pos, vel, grav, plane, ent in ecs.each(it) do
        local planeEpsilon = 0.1
		
		if not (plane.x * pos.x + plane.y * pos.y + plane.z * pos.z < plane.w + planeEpsilon) then
			vel.x = vel.x + grav.x * it.delta_time
			vel.y = vel.y + grav.y * it.delta_time
			vel.z = vel.z + grav.z * it.delta_time
		end
    end
end

local function FrictionSystem(it)
    for vel, friction, ent in ecs.each(it) do
        vel.x = vel.x - vel.x * friction.value * it.delta_time
		vel.y = vel.y - vel.y * friction.value * it.delta_time
		vel.z = vel.z - vel.z * friction.value * it.delta_time
    end
end

local function ShiverSystem(it)
    for pos, shiver, ent in ecs.each(it) do
        pos.x = pos.x + rand_flt(-shiver.value, shiver.value)
		pos.y = pos.y + rand_flt(-shiver.value, shiver.value)
		pos.z = pos.z + rand_flt(-shiver.value, shiver.value)
    end
end

local function BounceSystem(it)
    for pos, vel, plane, bounciness, ent in ecs.each(it) do
        local dotPos = plane.x * pos.x + plane.y * pos.y + plane.z * pos.z
		local dotVel = plane.x * vel.x + plane.y * vel.y + plane.z * vel.z
		
		if dotPos < plane.w then
			pos.x = pos.x - (dotPos - plane.w) * plane.x
			pos.y = pos.y - (dotPos - plane.w) * plane.y
			pos.z = pos.z - (dotPos - plane.w) * plane.z

			vel.x = vel.x - (1.0 + bounciness.value) * plane.x * dotVel
			vel.y = vel.y - (1.0 + bounciness.value) * plane.y * dotVel
			vel.z = vel.z - (1.0 + bounciness.value) * plane.z * dotVel
		end
    end
end

local function DespawnSystem(it)
	for timer, ent in ecs.each(it) do
		if timer.isTriggered then
			timer.timeToDespawn = timer.timeToDespawn - it.delta_time;
		end

		-- Have to delay entity destruction in order to move it through RenderObjectPtr
		if timer.isReadyForDestruction and timer.timeToDespawn < -0.12 then
			ecs.delete(ent)
		end
	end
end


local function Length(pos1, pos2)
	local dx = pos1.x - pos2.x
	local dy = pos1.y - pos2.y
	local dz = pos1.z - pos2.z

	return math.sqrt(dx * dx + dy * dy + dz * dz)
end

local collision_query = ecs.query("CollisionTraits, Position")
-- local quary_it = ecs.query_iter(collision_query)

local function CollisionSystem(it)
	local dt = it.delta_time

	-- while ecs.query_next(quary_it) do
	-- 	local collision2, pos2, ent2 = ecs.columns(quary_it)
	-- 	for collision1, pos1, ent1 in ecs.each(it) do
	for collision1, pos1, ent1 in ecs.each(it) do
		-- for collision2, pos2, ent2 in ecs.each(collision_query) do
		local quary_it = ecs.query_iter(collision_query)
        while ecs.query_next(quary_it) do
			for collision2, pos2, ent2 in ecs.each(quary_it) do
				if ent1 ~= ent2 then
                    local dist = Length(pos1, pos2)
                    local radiusSum = collision1.radius + collision2.radius

					if dist < radiusSum and collision1.delay < 0.0 and collision2.delay < 0.0 then
						collision1.isCollided = true
						collision2.isCollided = true
					end
				end
			end
		end
	end
end

-- local CollisionTraits = ecs.lookup("CollisionTraits")

-- local function CollisionSystem(it)
--     local entities = {}

--     for collision, pos, ent in ecs.each(it) do
--         table.insert(entities, {
--             ent = ent,
--             pos = { x = pos.x, y = pos.y, z = pos.z },
--             radius = collision.radius,
-- 			delay = collision.delay,
--         })
--     end

-- 	for i = 1, #entities do
--         for j = i + 1, #entities do
-- 			local e1 = entities[i]
-- 			local e2 = entities[j]

-- 			if e1.ent ~= e2.ent then

-- 				local dist = Length(e1.pos, e2.pos)
-- 				local radiusSum = e1.radius + e2.radius

-- 				-- local dt = it.delta_time
-- 				-- e1.delay = e1.delay - dt
-- 				-- e2.delay = e2.delay - dt

-- 				if dist < radiusSum and e1.delay < 0.0 and e2.delay < 0.0 then
-- 					-- ecs.get_mut(e1.ent, CollisionTraits).isCollided = true;
-- 					-- ecs.get_mut(e2.ent, CollisionTraits).isCollided = true;
-- 					-- e1.collision.isCollided = true;
-- 					-- e2.collision.isCollided = true;
-- 					-- ecs.set(e1.ent, CollisionTraits, {true, e1.radius, 0.5}) = true;
-- 					-- ecs.set(e2.ent, CollisionTraits, {true, e2.radius, 0.5}) = true;
-- 					local c1 = ecs.get(e1.ent, CollisionTraits)
-- 					if c1 then
-- 						ecs.set(e1.ent, CollisionTraits, { isCollided = true, radius = c1.radius, delay = 5.5 })
-- 					end

-- 					local c2 = ecs.get(e2.ent, CollisionTraits)
-- 					if c2 then
-- 						ecs.set(e2.ent, CollisionTraits, { isCollided = true, radius = c2.radius, delay = 5.5 })
-- 					end
-- 				end
-- 			end
--         end
--     end
-- end

local function CollisionDelaySystem(it)
    for collision, ent in ecs.each(it) do
		collision.delay = collision.delay - it.delta_time
    end
end

local function TriggerMovingXSystem(it)
	for collision, velocity, despawn in ecs.each(it) do
		if collision.isCollided then
			despawn.isTriggered = true
			velocity.x = -velocity.x
			collision.isCollided = false
			collision.delay = 0.3
		end
	end
end

local function GettingHitSystem(it)
	for collision, velocity in ecs.each(it) do
		if collision.isCollided then
			velocity.y = velocity.y + 5.0
			-- ecs.remove(ent, CollisionTraits)
			collision.isCollided = false
			collision.delay = 0.3
		end
	end
end

-- local function CollisionTriggerSystem(it)
-- 	for collision, despawn, ent in ecs.each(it) do
-- 		if collision.isCollided then
-- 			despawn.isTriggered = true
-- 			-- ecs.remove(ent, CollisionTraits)
-- 			collision.isCollided = false
-- 			collision.delay = 0.3;
-- 		end
-- 	end
-- end

ecs.system(move, "Move", ecs.OnUpdate, "Position, Velocity")
ecs.system(gravity, "grav", ecs.OnUpdate, "Position, Velocity, Gravity, BouncePlane")
ecs.system(FrictionSystem, "FrictionSystem", ecs.OnUpdate, "Velocity, FrictionAmount")
ecs.system(ShiverSystem, "ShiverSystem", ecs.OnUpdate, "Position, ShiverAmount")
ecs.system(BounceSystem, "BounceSystem", ecs.OnUpdate, "Position, Velocity, BouncePlane, Bounciness")
ecs.system(DespawnSystem, "Countdown", ecs.OnUpdate, "DespawnTimer")
ecs.system(CollisionSystem, "Collision", ecs.OnUpdate, "CollisionTraits, Position")
ecs.system(CollisionDelaySystem, "CollisionDelay", ecs.OnUpdate, "CollisionTraits")
ecs.system(TriggerMovingXSystem, "TriggerMovingX", ecs.OnUpdate, "CollisionTraits, Velocity, DespawnTimer, MovingOnX")
ecs.system(GettingHitSystem, "Hit", ecs.OnUpdate, "CollisionTraits, Velocity, Target")
-- ecs.system(CollisionTriggerSystem, "CollisionTrigger", ecs.OnUpdate, "CollisionTraits, DespawnTimer")


