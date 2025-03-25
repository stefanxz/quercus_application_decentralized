Module T1 = {
    .id = 1,
    .lookup = {-1, 0, 0, 3, 4, 4, 4, 4},
    //.plane_to_id = {-1, -1, -1, -1},
    .dropoff_id = -1,
    .quarantine_id = -1,
    .security_id = -1,
    .storage_id = 4,
    .is_storage = false,
    .current = 0,
    .next_free = 0,
    .next = {4, -1, -1}
};
Module T2 = {
    .id = 2,
    .lookup = {-1, 0, 0, 5, 4, 5, 5, 5},
    //.plane_to_id = {-1, -1, -1, -1},
    .dropoff_id = -1,
    .quarantine_id = -1,
    .security_id = -1,
    .storage_id = 5,
    .is_storage = true,
    .current = 0,
    .next_free = 0,
    .next = {5, 3, 7}
};          
Module T3 = {
    .id = 3,
    .lookup = {-1, 0, 0, 6, 6, 5, 6, 6},
    //.plane_to_id = {-1, -1, -1, -1},
    .dropoff_id = -1,
    .quarantine_id = -1,
    .security_id = -1,
    .storage_id = 6,
    .is_storage = true,
    .current = 0,
    .next_free = 0,
    .next = {6, -1, 4}
};
Module T4 = {
    .id = 4,
    .lookup = {-1, 0, 0, 7, 7, 7, 6, 7},
    //.plane_to_id = {-1, -1, -1, -1},
    .dropoff_id = -1,
    .quarantine_id = -1,
    .security_id = -1,
    .storage_id = 7,
    .is_storage = true,
    .current = 0,
    .next_free = 0,
    .next = {7, -1, 5}
};
Module T5 = {
    .id = 5,
    .lookup = {-1, 0, 0, 4, 4, 4, 4, 7},
    //.plane_to_id = {-1, -1, -1, -1},
    .dropoff_id = -1,
    .quarantine_id = -1,
    .security_id = -1,
    .storage_id = 4,
    .is_storage = true,
    .current = 0,
    .next_free = 0,
    .next = {4, -1, 6}
};

// Unused for now
Module T6 = {
    .id = 6,
    .lookup = {1, 2, 3},
    //.plane_to_id = {-1, -1, -1, -1},
    .dropoff_id = 1,
    .quarantine_id = 2,
    .security_id = 3,
    .storage_id = 4,
    .is_storage = true,
    .current = 0,
    .next_free = 0,
    .next = {1, 2, 3}
};
Module T7 = {
    .id = 7,
    .lookup = {1, 2, 3},
    //.plane_to_id = {-1, -1, -1, -1},
    .dropoff_id = 1,
    .quarantine_id = 2,
    .security_id = 3,
    .storage_id = 4,
    .is_storage = true,
    .current = 0,
    .next_free = 0,
    .next = {1, 2, 3}
};
