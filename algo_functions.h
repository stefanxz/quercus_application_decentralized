#define export __attribute__((visibility("default"))) __attribute__((used))

#define MAX_NUMBER_OF_MODULES 256

typedef struct
{
	int id;
	int lookup[MAX_NUMBER_OF_MODULES];
	int dropoff_id;
	int quarantine_id;
	int security_id;
	int storage_id;
	Tub tub;
	// Index 0 will be plane on this module
	int plane_to_id[256];
} Module;

typedef struct
{
    int id;
	int plane_id;
	bool plane_dropoff;
    // int priority;
    int passed_security;
    int destination;
	bool plane_arrived;
} Tub;