SET SERVEROUTPUT ON;
DECLARE
   num NUMBER := 2;
   n NUMBER := &n;
   i NUMBER;
   is_prime BOOLEAN;
BEGIN
   WHILE num <= n LOOP
      is_prime := TRUE;  

      FOR i IN 2 .. num - 1 LOOP  -- Check divisibility
         IF num MOD i = 0 THEN
            is_prime := FALSE;
            EXIT;
         END IF;
      END LOOP;

      IF is_prime THEN
         dbms_output.put_line(num);
      END IF;

      num := num + 1;  
   END LOOP;
END;
/
