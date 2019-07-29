import global.exception.ReaderException;
import global.tools.Reset;
import reader.Reader;
import constructor.Constructor;
import extractor.Extractor;
import generator.Generator;

public class Singularity {
    //Attributes
    private Reader reader;
    private Extractor extractor;
    private Constructor constructor;
    private Generator generator;
    private Reset reset;

    //Constructor
    public Singularity () {
        reader = new Reader();
        extractor = new Extractor();
        constructor = new Constructor();
        generator = new Generator();
        reset = new Reset();
    }

    //Methods
    public void run (String input) {
        try {
            extractor.extract(reader.read(input));
        }catch (ReaderException exception) {
            exception.printStackTrace();
        }
        reset.masterReset();
    }

}
