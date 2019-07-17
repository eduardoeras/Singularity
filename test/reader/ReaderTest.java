package reader;

import org.antlr.v4.runtime.tree.ParseTree;

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

import java.io.IOException;

public class ReaderTest {

    //Attributes
    Reader reader;
    ParseTreePrinter parseTreePrinter;

    @Before
    public void init () {
        reader = new Reader();
        parseTreePrinter = new ParseTreePrinter();
    }

    @Test
    public void testReader () {
        ParseTree parseTree;
        try {
            parseTree = reader.read("test/data/triangle.cpp");
            parseTreePrinter.printEverything(parseTree);
        }catch (IOException e) {
            System.out.println(e.toString());
            Assert.fail();
        }
    }

}
