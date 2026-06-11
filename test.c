#include <windows.h>
#include <stdio.h>

// Simple test target for memory scanner
// Run this, then use your scanner to find and modify these values

int main(void) {
   
    printf("Memory Scanner Test Target\n");
    printf("===========================\n\n");
    printf("PID: %lu\n\n", GetCurrentProcessId());
    
    // Different values to scan for
    unsigned int health = 100;
    unsigned int ammo = 50;
    unsigned int score = 12345;
    unsigned int coins = 999;
    
    printf("Initial values:\n");
    printf("  Health: %u (address: %p)\n", health, (void*)&health);
    printf("  Ammo:   %u (address: %p)\n", ammo, (void*)&ammo);
    printf("  Score:  %u (address: %p)\n", score, (void*)&score);
    printf("  Coins:  %u (address: %p)\n\n", coins, (void*)&coins);
    
    printf("Instructions:\n");
    printf("1. Use your scanner to search for value 100\n");
    printf("2. Press ENTER to change health to 75\n");
    printf("3. Use 'Next Scan' to find addresses that changed\n");
    printf("4. Try writing to the health address\n\n");
    
    printf("Press ENTER to continue...\n");
    getchar();
    
    // Change health
    health = 75;
    printf("\nHealth changed to: %u\n", health);
    printf("Press ENTER to change health to 50...\n");
    getchar();
    
    // Change again
    health = 50;
    printf("Health changed to: %u\n", health);
    printf("Press ENTER to change health to 25...\n");
    getchar();
    
    // Final change
    health = 25;
    printf("Health changed to: %u\n\n", health);
    
    printf("Now try these tests:\n");
    printf("1. Scan for 999 (coins) - should find it\n");
    printf("2. Scan for 12345 (score) - should find it\n");
    printf("3. Write a new value to any address\n\n");
    
    // Loop showing current values
    printf("Monitoring values (press Ctrl+C to exit):\n");
    printf("=========================================\n");
    
    while(1) {
        // Clear previous line (simple version)
        printf("\rHealth: %3u | Ammo: %3u | Score: %5u | Coins: %3u  ", 
               health, ammo, score, coins);
        fflush(stdout);
        Sleep(1000);  // Update every second
    }
    
    return 0;
}