package global.tools;

public class FileName {
    //Attrbutes
    private String fileName;
    private static FileName uniqueInstance;

    //Constructor
    private FileName () {
        fileName = "";
    }

    //Static Methods
    public static FileName getInstance() {
        if (uniqueInstance == null) {
            uniqueInstance = new FileName();
        }
        return uniqueInstance;
    }

    public void setFileName (String fileName) {
        this.fileName = fileName.substring(0, fileName.indexOf('.'));
    }

    public String getFileName () {
        return fileName;
    }

    public void reset () {
        fileName = "";
    }
}
