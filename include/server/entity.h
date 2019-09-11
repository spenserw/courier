#ifndef ENTITY_H
#define ENTITY_H

#include "util.h"

#ifdef COURIER_CLIENT
#include "graphics.h"
#define SPRITE_DEF() Sprite sprite;
#else
#define SPRITE_DEF()
#endif

enum EntityType {
      ENTITY_BASE=0,
      ENTITY_ACTOR,

      ENTITY_STRUCTURE,
      ENTITY_MINE,

      ENTITY_UNIT,
      ENTITY_BUGGY
};

#define ENTITY_TYPE(etype) static EntityType type () { return etype; }

/**
 * Entity - Base class for all ephemeral entities
 *
 * `size_t _instance_id`: `_instance_id` identifies a unique entity
 */
class Entity {
public:
    ENTITY_TYPE(ENTITY_BASE)
protected:
    size_t _instance_id;
};

// Actor implies a tangible in-game actor (unit, building, etc.)
class Actor : public Entity {
public:
    ENTITY_TYPE(ENTITY_ACTOR)
    
protected:
    Vec3f position;
    size_t sight_radius;
};

// Structure implies a stationary actor
class Structure : public Actor {
public:
    ENTITY_TYPE(ENTITY_STRUCTURE)
};

class Mine : public Actor {
    ENTITY_TYPE(ENTITY_MINE)
};

// Unit implies a military actor with or without attacking capabilities
class Unit : public Actor {
public:
    ENTITY_TYPE(ENTITY_UNIT)
};

class Buggy : public Unit {
    ENTITY_TYPE(ENTITY_BUGGY)
};

#endif /* ENTITY_H */
