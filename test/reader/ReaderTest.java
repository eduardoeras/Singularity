package reader;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;

public class ReaderTest {

    //Attributes
    Reader reader;

    @Before
    public void init () {
        reader = new Reader();
    }

    @Test
    public void testReader () {
        try {
            reader.read("test/resources/polymorphic_pointers.cpp");
        }catch (IOException e) {
            System.out.println(e.toString());
            Assert.fail();
        }
    }

}
