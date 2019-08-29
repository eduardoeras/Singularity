package extractor.gears;

import global.structure.State;
import global.structure.Element;
import global.structure.Type;
import global.structure.Visibility;

import java.io.PrintWriter;
import java.util.List;

public class StatePrinter {
    //Attrbutes
    private String fileName;
    private static StatePrinter uniqueInstance;

    //Constructor
    private StatePrinter () {
        fileName = "";
    }

    //Static Methods
    public static StatePrinter getInstance() {
        if (uniqueInstance == null) {
            uniqueInstance = new StatePrinter();
        }
        return uniqueInstance;
    }

    //Public Methods
    public void print (List<State> states) {
        String output = "";
        for (State state : states) {
            String line = "";
            for (int i = 0; i < state.getScopeLevel(); i++) {
                line = line.concat("    ");
            }

            if (state.getType() == Type.STATE) {
                line = line.concat("<state label='");
            } else {
                if (state.getElement() == Element.NONE) {
                    line = line.concat("</level>");
                    output = output.concat(line + "\n");
                    continue;
                } else {
                    line = line.concat("<level label='");
                }
            }

            line = line.concat(state.getLabel());
            line = line.concat("' element='");
            line = line.concat(state.getElement().toString());

            if (state.getVisibility() != Visibility.NONE) {
                line = line.concat("' visibility='");
                line = line.concat(state.getVisibility().toString());
            }

            if (state.getId() != -1) {
                line = line.concat("' id='");
                line = line.concat(String.valueOf(state.getId()));
            }

            line = line.concat("'>");
            output = output.concat(line + "\n");
        }

        System.out.print(output);
        try {
            PrintWriter printWriter = new PrintWriter(fileName + ".xml");
            printWriter.print(output);
            printWriter.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void setFileName (String fileName) {
        this.fileName = fileName;
    }

    public void reset () {
        fileName = "";
    }
}
