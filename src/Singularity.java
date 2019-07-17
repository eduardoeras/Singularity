import reader.Reader;
import constructor.Constructor;
import extractor.Extractor;
import generator.Generator;

import java.io.IOException;

public class Singularity {
    //Attributes
    private Reader reader;
    private Extractor extractor;
    private Constructor constructor;
    private Generator generator;

    //Constructor
    public Singularity () {
        reader = new Reader();
        extractor = new Extractor();
        constructor = new Constructor();
        generator = new Generator();
    }

    //Methods
    public void run (String input) {
        try {
            extractor.extract(reader.read(input));
        }catch (IOException exception) {
            System.out.println("ERROR: Input file not found.");
            exception.printStackTrace();
        }
    }

}
