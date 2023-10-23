# Overview


# System calls

## System call list

### map_anonymous(location, size, flags)



Privilage level: user

### unmap_anonymous(location, size)

Privilage level: user

### map_physical(location, size)

Privilage level: system

### unmap_physical(location, size)

Privilage level: system

### object_create(location, size, flags)

Privilage level: user

### object_aquire(id, location)

Privilage level: user

### object_release(id)

Privilage level: user

### send_pid(data, dest, flags)

Privilage level: user

### send_port(data, dest, flags)

Privilage level: user

### receive(buffer, flags)

Privilage level: user

### port_open(id)

Privilage level: user

### port_close(id)

Privilage level: user

### clone(entry, arg, stack, flags)

Privilage level: user

### signal_action(id, action, flags)

Privilage level: user

### signal_return()

Privilage level: user

### signal_raise(pid, id, arg)

Privilage level: user

### interrupt_action(id, action, flags)

Privilage level: system

### interrupt_return()

Privilage level: system
