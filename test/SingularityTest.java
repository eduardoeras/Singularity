import org.junit.Before;
import org.junit.Test;

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
    public void controlTests () {
        System.out.println("\n          MINIMAL\n");
        singularity.run("test/data/minimal.cpp", true);
        System.out.println("\n          ALLCONDITIONS\n");
        singularity.run("test/data/allConditions.cpp", false);
        System.out.println("\n          TRIANGLE\n");
        singularity.run("test/data/triangle.cpp", false);
        System.out.println("\n          POLYMORPHICPOINTERS\n");
        singularity.run("test/data/polymorphic_pointers.cpp", false);
    }
    //@Test
    public void geoDmaBasics () {
        System.out.println("\n          CONTEXT\n");
        singularity.run("test/data/context.cpp", true);
        System.out.println("\n          GENERALFUNCTIONS\n");
        singularity.run("test/data/generalFunctions.cpp", true);
        System.out.println("\n          INTERSECTIONCACHE\n");
        singularity.run("test/data/intersectionCache.cpp", true);
        System.out.println("\n          MULTIGEOMETRYALGORITHMS\n");
        singularity.run("test/data/multiGeometryAlgorithms.cpp", true);
    }

    //@Test
    public void bigOnes () {
        System.out.println("\n          GEOMETRYFUNCTIONS\n");
        singularity.run("test/data/geometryFunctions.cpp", true);
        System.out.println("\n          LANDSCAPEFEATURES\n");
        singularity.run("test/data/landscapeFeatures.cpp", true);
    }

    //@Test
    public void exceptionTest () {
        singularity.run("none", true);
    }
}
