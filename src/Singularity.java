import global.exception.ReaderException;
import global.tools.FileName;
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
    private FileName fileName;
    private Reset reset;

    //Constructor
    public Singularity () {
        reader = new Reader();
        extractor = new Extractor();
        constructor = new Constructor();
        generator = new Generator();
        fileName = FileName.getInstance();
        reset = new Reset();
    }

    //Methods
    public void run (String input, boolean debug) {
        fileName.setFileName(input);
        try {
            extractor.extract(reader.read(input), debug);
            constructor.build(generator.generate(extractor.getStates(), extractor.getTransitions()));
        } catch (ReaderException exception) {
            exception.printStackTrace();
        }
        reset.masterReset();
    }

}
