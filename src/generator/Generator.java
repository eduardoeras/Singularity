package generator;

import global.structure.State;
import global.structure.Transition;
import global.tools.FileName;

import java.io.PrintWriter;
import java.util.List;

public class Generator {
    //Attributes
    private FileName fileName;

    //Constructor
    public Generator () {
        fileName = FileName.getInstance();
    }

    //Public Methods
    public void generate (List<State> states, List<Transition> transitions) {
        String output = "MODULE main\n" +
                "\n" +
                "    VAR\n" +
                "        state : \n" +
                "        {\n" +
                "            \n" +
                "        };\n" +
                "\n" +
                "        events :\n" +
                "        {\n" +
                "\n" +
                "        };\n" +
                "\n" +
                "        decision : boolean;\n" +
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
                "            esac;\n" +
                "\n" +
                "        next(decision) :=\n" +
                "            case\n" +
                "                \n" +
                "                TRUE : {TRUE, FALSE};\n" +
                "            esac;";
        try {
            PrintWriter printWriter = new PrintWriter(fileName.getFileName() + ".smv");
            printWriter.print(output);
            printWriter.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
