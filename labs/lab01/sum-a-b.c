#include "json-c/json.h"
#include <stdio.h>

int main(void)
{
   json_object *root = json_object_from_file("sum-input.json");
   if (!root) {
      perror("sum-input.json");
      exit(1);
   }

   json_object *obj_a, *obj_b;
   obj_a = json_object_object_get(root, "a");
   obj_b = json_object_object_get(root, "b");

   double a, b;
   a = json_object_get_double(obj_a);
   b = json_object_get_double(obj_b);

   printf("%f + %f = %f\n", a, b, (a + b));
   return 0;
}