package reader;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;

public class ReaderAntlrTest {

    //Attributes
    ReaderAntlr readerAntlr;

    @Before
    public void init () {
        readerAntlr = new ReaderAntlr();
    }

    @Test
    public void testReader () {
        try {
            readerAntlr.read("test/data/polymorphic_pointers.cpp");
        }catch (IOException e) {
            System.out.println(e.toString());
            Assert.fail();
        }
    }

}
