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
    //@Test
    public void controlTests () {
        System.out.println("\n          MINIMAL\n");
        singularity.run("test/data/minimal.cpp", false);
        System.out.println("\n          ALLCONDITIONS\n");
        singularity.run("test/data/allConditions.cpp", false);
        System.out.println("\n          TRIANGLE\n");
        singularity.run("test/data/triangle.cpp", false);
    }

    @Test
    public void geoDmaSelection () {
        System.out.println("\n          ANALYSIS\n");
        singularity.run("test/GeoDMA_Selection/analysis.cpp", false);
        System.out.println("\n          ARITHMETICVERTICE\n");
        singularity.run("test/GeoDMA_Selection/arithmeticVertice.cpp", false);
        System.out.println("\n          ATTRIBUTE\n");
        singularity.run("test/GeoDMA_Selection/attribute.cpp", false);
        System.out.println("\n          CLASSDESCRIPTION\n");
        singularity.run("test/GeoDMA_Selection/classDescription.cpp", false);
        System.out.println("\n          CLASSDESCRIPTIONSET\n");
        singularity.run("test/GeoDMA_Selection/classDescriptionSet.cpp", false);
        System.out.println("\n          COMMANDLINEPROGVIEWER\n");
        singularity.run("test/GeoDMA_Selection/commandLineProgViewer.cpp", false);
        System.out.println("\n          CONTEXT\n");
        singularity.run("test/GeoDMA_Selection/context.cpp", false);
        System.out.println("\n          DUMMYVERTICE\n");
        singularity.run("test/GeoDMA_Selection/dummyVertice.cpp", false);
        System.out.println("\n          EXECUTER\n");
        singularity.run("test/GeoDMA_Selection/executer.cpp", false);
        System.out.println("\n          FEATURESSUMMARY\n");
        singularity.run("test/GeoDMA_Selection/featuresSummary.cpp", false);
        System.out.println("\n          FEATURESVERTICE\n");
        singularity.run("test/GeoDMA_Selection/featuresVertice.cpp", false);
        System.out.println("\n          GENERALFUNCTIONS\n");
        singularity.run("test/GeoDMA_Selection/generalFunctions.cpp", false);
        System.out.println("\n          GRAPH\n");
        singularity.run("test/GeoDMA_Selection/graph.cpp", false);
        System.out.println("\n          GRAPHEXECUTER\n");
        singularity.run("test/GeoDMA_Selection/graphExecuter.cpp", false);
        System.out.println("\n          GRAPHVERTICE\n");
        singularity.run("test/GeoDMA_Selection/graphVertice.cpp", false);
        System.out.println("\n          IMPORTVERTICE\n");
        singularity.run("test/GeoDMA_Selection/importVertice.cpp", false);
        System.out.println("\n          INTERSECTIONCACHE\n");
        singularity.run("test/GeoDMA_Selection/intersectionCache.cpp", false);
        System.out.println("\n          LOCALEXECUTER\n");
        singularity.run("test/GeoDMA_Selection/localExecuter.cpp", false);
        System.out.println("\n          MAPVERTICE\n");
        singularity.run("test/GeoDMA_Selection/mapVertice.cpp", false);
        System.out.println("\n          PROJECT\n");
        singularity.run("test/GeoDMA_Selection/project.cpp", false);
        System.out.println("\n          SCRIPTVERTICE\n");
        singularity.run("test/GeoDMA_Selection/scriptVertice.cpp", false);
        System.out.println("\n          SEGMENTERVERTICE\n");
        singularity.run("test/GeoDMA_Selection/segmenterVertice.cpp", false);
        System.out.println("\n          SELECTVERTICE\n");
        singularity.run("test/GeoDMA_Selection/selectVertice.cpp", false);
        System.out.println("\n          SPATIALFEATURES\n"); //NuSMV ERROR
        singularity.run("test/GeoDMA_Selection/spatialFeatures.cpp", false);
    }

    //@Test
    public void exceptionTest () {
        singularity.run("none", true);
    }
}
