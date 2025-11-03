#include "../../libs/API.h"
#include <stdio.h>

int main()
{
    printf("=== Simple Geocoding API - Get JSON Data ===\n\n");
    
    const char* city = "Hamburg";
    
    printf("Requesting geocoding data for: %s\n\n", city);
    
    // Using coordinates for Hamburg, Germany area (53.55, 9.99)
    char* json = API_get_geocoding_json(city, 53.55, 9.99);
    
    if (json != NULL) {
        printf("\n=== RECEIVED JSON DATA ===\n");
        printf("%s\n", json);
        printf("=== END JSON DATA ===\n\n");
        
        printf("You can now parse this JSON data however you want!\n");
        printf("For example, you could:\n");
        printf("- Use a JSON library to parse it\n");
        printf("- Extract specific fields manually\n");
        printf("- Store it to a file\n");
        printf("- Pass it to another function\n");
        
        // Clean up
        API_free_json(json);
        printf("\nJSON data freed successfully\n");
    } else {
        printf("Failed to get geocoding data for '%s'\n", city);
        return 1;
    }
    
    printf("\nGeocode API example completed successfully!\n");
    return 0;
}