package reader;

import org.junit.Before;
import org.junit.Test;

public class ReaderTest {
    //Attributes
    private Reader reader;

    //Constructor
    @Before
    public void init () {
        reader = new Reader();
    }

    //Methods
    @Test
    public void testRead () {
        reader.read("test/data/polymorphic_pointers.cpp");
    }
}
