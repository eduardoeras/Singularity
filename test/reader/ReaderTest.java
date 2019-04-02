package reader;

import org.junit.Before;
import org.junit.Test;

public class ReaderTest {
    //Attributes
    Reader reader;
    //Constructor
    @Before
    public void init () {
        reader = new Reader();
    }
    //Methods
    @Test
    public void testRead () {
        reader.read("test/resources/polymorphic_pointers.cpp");
    }
}
