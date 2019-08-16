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
        /*System.out.println("\n          ALLCONDITIONS\n");
        singularity.run("test/data/allConditions.cpp");
        System.out.println("\n          TRIANGLE\n");
        singularity.run("test/data/triangle.cpp");*/
        System.out.println("\n          CONTEXT\n");
        singularity.run("test/data/context.cpp");
        System.out.println("\n          GENERALFUNCTIONS\n");
        singularity.run("test/data/generalFunctions.cpp");
        System.out.println("\n          INTERSECTIONCACHE\n");
        singularity.run("test/data/intersectionCache.cpp");
    }

    //@Test
    public void exceptionTest () {
        singularity.run("none");
    }
}
