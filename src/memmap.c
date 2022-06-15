#include "memmap.h"
#include <stdbool.h>

int compare_regions(struct memory_region_t *lhs, struct memory_region_t *rhs)
{
    if(lhs->location == rhs->location)
    {
        return lhs->size > rhs->size ? 1
            : (lhs->size == rhs->size ? 0
            : -1);
    }
    else
    {
        return lhs->location > rhs->location ? 1 : -1;
    }
}

bool region_overlaps(struct memory_region_t *lhs, struct memory_region_t *rhs)
{
    if(rhs->location < lhs->location)
    {
        return rhs->location + rhs->size > lhs->location;
    }
    return rhs->location < lhs->location + lhs->size;
}

bool region_contains(struct memory_region_t *lhs, struct memory_region_t *rhs)
{
    return (rhs->location >= lhs->location) &&
        (rhs->location + rhs->size <= lhs->location + lhs->size);
}

void insert_map_entry(struct memory_map_t *map, physaddr_t location, size_t size, unsigned int type)
{
    struct memory_region_t new_region = {.location = location, .size = size, .type = type};
    unsigned int i = 0;
    while(i < map->size)
    {
        if(compare_regions(&new_region, &map->array[i]) < 0)
        {
            struct memory_region_t buffer = new_region;
            new_region = map->array[i];
            map->array[i] = buffer;
        }
        i++;
    }
    map->array[i] = new_region;
    map->size++;
}

void remove_map_entry(struct memory_map_t *map, int index)
{
    if(index >= 0 && index < map->size)
    {
        for(int i = index; i < map->size - 1; i++)
        {
            map->array[i] = map->array[i + 1];
        }
        map->size--;
    }
}

int trim_map(struct memory_map_t *map, int index)
{
    if(index + 1 >= map->size)
    {
        return -1;
    }
    struct memory_region_t *left = &map->array[index];
    struct memory_region_t *right = &map->array[index + 1];
    if(region_overlaps(left, right))
    {
        if(left->type == right->type)
        {
            left->size = (right->location + right->size > left->location + left->size ? right->location + right->size : left->location + left->size) - left->location;
            remove_map_entry(map, index + 1);
            return index;
        }
        else if(left->type < right->type)
        {
            if(region_contains(right, left))
            {
                remove_map_entry(map, index);
                return index;
            }
            else if(left->location + left->size <= right->location + right->size)
            {
                left->size = (right->location > left->location) ? right->location - left->location : 0;
                return index + 1;
            }
            else
            {
                struct memory_region_t new_right = {
                    .location = right->location + right->size, 
                    .size = (left->location + left->size) - (right->location + right->size), 
                    .type = left->type};
                left->size = (right->location > left->location) ? right->location - left->location : 0;
                if(left->size == 0)
                    remove_map_entry(map, index);
                insert_map_entry(map, new_right.location, new_right.size, new_right.type);
                return index + 2;
            }
        }
        else
        {
            if(region_contains(left, right))
            {
                remove_map_entry(map, index + 1);
                return index;
            }
            else
            {
                right->size = (right->location + right->size) - (left->location + left->size);
                right->location = left->location + left->size;
                return index + 1;
            }
        }
    }
    else if((left->location + left->size == right->location) && left->type == right->type)
    {
        left->size = right->location + right->size - left->location;
        remove_map_entry(map, index + 1);
        return index;
    }
    return index + 1;
}

void insert_region(struct memory_map_t *map, physaddr_t location, size_t size, unsigned int type)
{
    insert_map_entry(map, location, size, type);
    int i = 0;
    while(i >= 0)
    {
        i = trim_map(map, i);
    }
}
