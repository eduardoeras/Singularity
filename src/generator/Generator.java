package generator;

import generator.elements.Variables;
import global.structure.State;
import global.structure.Transition;
import global.tools.FileName;

import java.io.PrintWriter;
import java.util.List;

public class Generator {
    //Attributes
    private FileName fileName;
    private Variables variables;

    //Constructor
    public Generator () {
        fileName = FileName.getInstance();
        variables = new Variables();
    }

    //Public Methods
    public void generate (List<State> states, List<Transition> transitions) {
        boolean existsBoolean = variables.existitsBoolean(transitions);
        String output = "";
        output = output.concat(
                "MODULE main\n" +
                "\n" +
                "    VAR\n" +
                "        state : \n" +
                "        {\n");
        output = output.concat(variables.getStates(states));
        output = output.concat(
                "        };\n" +
                "\n" +
                "        events :\n" +
                "        {\n");
        output = output.concat(variables.getEvents(transitions));
        output = output.concat("        };\n");
        if (existsBoolean) {
            output = output.concat("\n        decision : boolean;\n");
        }
        output = output.concat(
                "\n" +
                "    ASSIGN\n" +
                "\n" +
                "        init(state) := ;\n" +
                "\n" +
                "        next(state) :=\n" +
                "            case\n" +
                "                \n" +
                "                TRUE : state;\n" +
                "            esac;\n" +
                "\n" +
                "        next(events) :=\n" +
                "            case\n" +
                "                \n" +
                "                TRUE : events;\n" +
                "            esac;");
        if (existsBoolean) {
            output = output.concat(
                    "\n\n" +
                    "        next(decision) :=\n" +
                    "            case\n" +
                    "                \n" +
                    "                TRUE : {TRUE, FALSE};\n" +
                    "            esac;");
        }
        try {
            PrintWriter printWriter = new PrintWriter(fileName.getFileName() + ".smv");
            printWriter.print(output);
            printWriter.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
