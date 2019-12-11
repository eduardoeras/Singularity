package extractor.tools;

import global.structure.*;
import global.tools.FileName;

import java.io.PrintWriter;
import java.util.List;

public class XmlPrinter {
    //Attrbutes
    private FileName fileName;

    //Constructor
    public XmlPrinter() {
        fileName = FileName.getInstance();
    }

    //Public Methods
    public void print (List<State> states, List<Transition> transitions, boolean debug) {
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
        output = output.concat("\n");
        for (Transition transition : transitions) {
            output = output.concat("<transition from = '");
            output = output.concat(transition.getFrom().getLabel() + "_" + transition.getFrom().getId());
            output = output.concat("' to = '");
            output = output.concat(transition.getTo().getLabel() + "_" + transition.getTo().getId());
            output = output.concat("' event = '");
            output = output.concat(transition.getEvent().getEvent());
            output = output.concat("'>\n");
        }

        if (debug) {
            System.out.print(output);
        }

        try {
            PrintWriter printWriter = new PrintWriter(fileName.getFileName() + ".xml");
            printWriter.print(output);
            printWriter.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
