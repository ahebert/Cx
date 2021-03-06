/* standard type member functions example
 * 
 * var.size() - returns the integer byte size of var
 */

int main () {

    int i = 100;
    
    /* assigne the size of int to a
     * parens is optional for type members */
    int a = i.size();
    
    // i now equals 4 (sizeof(int) == 4 bytes)
    i = a.size;
    
    return 0;
}