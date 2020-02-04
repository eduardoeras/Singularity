import org.junit.Before;
import org.junit.Test;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.concurrent.TimeUnit;

public class SingularityTest {
    //Attributes
    Singularity singularity;

    //Constructor
    @Before
    public void init() {
        singularity = new Singularity();
    }

    //Methods
    @Test
    public void controlTests() {
        System.out.println("\n          MINIMAL\n");
        singularity.run("test/data/minimal.cpp", false);
        System.out.println("\n          ALLCONDITIONS\n");
        singularity.run("test/data/allConditions.cpp", false);
        System.out.println("\n          TRIANGLE\n");
        singularity.run("test/data/triangle.cpp", false);
    }

    @Test
    public void McCabe() {
        System.out.println("\n          CASE 01\n");
        singularity.run("test/McCabe/case01.cpp", false);
        System.out.println("\n          CASE 02\n");
        singularity.run("test/McCabe/case02.cpp", false);
        System.out.println("\n          CASE 03\n");
        singularity.run("test/McCabe/case03.cpp", false);
        System.out.println("\n          CASE 04\n");
        singularity.run("test/McCabe/case04.cpp", false);
        System.out.println("\n          CASE 05\n");
        singularity.run("test/McCabe/case05.cpp", false);
        System.out.println("\n          CASE 06\n");
        singularity.run("test/McCabe/case06.cpp", false);
        System.out.println("\n          CASE 07\n");
        singularity.run("test/McCabe/case07.cpp", false);
        System.out.println("\n          CASE 08\n");
        singularity.run("test/McCabe/case08.cpp", false);
        System.out.println("\n          CASE 09\n");
        singularity.run("test/McCabe/case09.cpp", false);
        System.out.println("\n          CASE 10\n");
        singularity.run("test/McCabe/case10.cpp", false);
        System.out.println("\n          CASE 11\n");
        singularity.run("test/McCabe/case11.cpp", false);
        System.out.println("\n          CASE 12\n");
        singularity.run("test/McCabe/case12.cpp", false);
        System.out.println("\n          CASE 13\n");
        singularity.run("test/McCabe/case13.cpp", false);
    }

    @Test
    public void geoDmaSelection() {
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
        System.out.println("\n          SPATIALFEATURES\n");
        singularity.run("test/GeoDMA_Selection/spatialFeatures.cpp", false);
    }

    //@Test
    public void terraLibSelection() throws InterruptedException {
        for (int i = 1000; i <= 1063; i++) { //Original number: 1466
            System.out.println(i);
            try {
                singularity.run("test/terralib_Selection/" + i + ".cpp", false);
            } catch (Exception e) {
                System.out.println("FAIL: " + i);
                /*String command;
                try {
                    command = "rm test/terralib_Selection/" + i + ".cpp";
                    Process process;
                    process = Runtime.getRuntime().exec(command);
                    process.waitFor();
                    command = "rm test/terralib_Selection/" + i + ".xml";
                    process = Runtime.getRuntime().exec(command);
                    process.waitFor();
                } catch (Exception w) {
                    w.printStackTrace();
                }*/
            }
            //System.gc();
            //TimeUnit.SECONDS.sleep(5);
        }
    }

}