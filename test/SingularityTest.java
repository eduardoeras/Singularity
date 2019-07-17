import org.junit.Before;
import org.junit.Test;

import java.io.IOException;

public class SingularityTest {
    //Attributes
    Singularity singularity;

    //Constructor
    @Before
    public void init () {
        singularity = new Singularity();
    }

    //Methods
    @Test
    public void mainTest () {
        singularity.run("test/data/triangle.cpp");
    }

    @Test
    public void exceptionTest () {
        singularity.run("none");
    }
}
