package global.tools;

public class Statistics {
    //Attributes
    private static Statistics uniqueInstance;
    private FileName fileName;
    private int states;
    private int eventTransitions;
    private int stateTransitions;
    private int events;
    private int decisions;
    private int components;
    private int properties;
    private int propertiesCaseOne;
    private int propertiesCaseTwo;
    private int propertiesCaseThree;
    private int totalCounterexamples;
    private int validCounterexamples;
    private int invalidCounterexamples;
    private int biggestCounterexample;
    private int smallestCounterexample;
    private int usedStates;
    private int usedtransitions;

    //Constructor
    private Statistics () {
        fileName = FileName.getInstance();
        reset();
    }

    //Methods
    public static Statistics getInstance() {
        if (uniqueInstance == null) {
            uniqueInstance = new Statistics();
        }
        return uniqueInstance;
    }

    public int getStates () {
        return states;
    }

    public void addState () {
        states ++;
    }

    public int getEventTransitions() {
        return eventTransitions;
    }

    public void addEventTransition() {
        eventTransitions ++;
    }

    public int getStateTransitions() {
        return stateTransitions;
    }

    public void addStateTransition() {
        stateTransitions ++;
    }

    public int getTotalTransitions() {
        return eventTransitions + stateTransitions;
    }

    public int getEvents() {
        return events;
    }

    public void addEvent() {
        events ++;
    }

    public int getDecisions() {
        return decisions;
    }

    public void addDecision() {
        decisions++;
    }

    public int getComponents () {
        return components;
    }

    public void addComponent () {
        components ++;
    }

    public int getProperties() {
        return properties;
    }

    public void addProperty() {
        properties ++;
    }

    public int getPropertiesCaseOne() {
        return propertiesCaseOne;
    }

    public void addPropertyCaseOne() {
        propertiesCaseOne ++;
    }

    public int getPropertiesCaseTwo() {
        return propertiesCaseTwo;
    }

    public void addPropertyCaseTwo() {
        propertiesCaseTwo ++;
    }

    public int getPropertiesCaseThree() {
        return propertiesCaseThree;
    }

    public void addPropertyCaseThree() {
        propertiesCaseThree ++;
    }

    public int getTotalCounterexamples() {
        return totalCounterexamples;
    }

    public void setTotalCounterexamples(int totalCounterexamples) {
        this.totalCounterexamples = totalCounterexamples;
    }

    public int getValidCounterexamples() {
        return validCounterexamples;
    }

    public void setValidCounterexamples(int validCounterexamples) {
        this.validCounterexamples = validCounterexamples;
    }

    public int getInvalidCounterexamples() {
        return invalidCounterexamples;
    }

    public void setInvalidCounterexamples(int invalidCounterexamples) {
        this.invalidCounterexamples = invalidCounterexamples;
    }

    public int getBiggestCounterexample () {
        return biggestCounterexample;
    }

    public void setBiggestCounterexample (int biggestCounterexample) {
        this.biggestCounterexample = biggestCounterexample;
    }

    public int getSmallestCounterexample () {
        return smallestCounterexample;
    }

    public void setSmallestCounterexample (int smallestCounterexample) {
        this.smallestCounterexample = smallestCounterexample;
    }

    public int getComplexity () {
        return stateTransitions - states + 2 * components;
    }

    public void setUsedStates (int usedStates) {
        this.usedStates = usedStates;
    }

    public void setUsedtransitions (int usedtransitions) {
        this.usedtransitions = usedtransitions;
    }

    public String print () {
        String header = "";

        header = header.concat("Name of the input C++ file: " + fileName.getFileName() + "\n");
        header = header.concat("Number of states: " + states + "\n");
        header = header.concat("Number of events: " + events + "\n");
        header = header.concat("Number of decisions: " + decisions + "\n");
        header = header.concat("Number of state transitions: " + stateTransitions + "\n");
        header = header.concat("Number of event transitions: " + eventTransitions + "\n");
        header = header.concat("Number of total transitions: " + getTotalTransitions() + "\n");
        header = header.concat("Number of components: " + components + "\n");
        header = header.concat("..................................\n");
        header = header.concat("Cyclomatic Complexity: " + getComplexity() + "\n");
        header = header.concat("..................................\n");
        header = header.concat("Number of Case One properties: " + propertiesCaseOne + "\n");
        header = header.concat("Number of Case Two properties: " + propertiesCaseTwo + "\n");
        header = header.concat("Number of Case Three properties: " + propertiesCaseThree + "\n");
        header = header.concat("Total Number of properties: " + properties + "\n");
        header = header.concat("..................................\n");
        header = header.concat("Number of Counterexamples: " + totalCounterexamples + "\n");
        header = header.concat("Number of Valid Counterexamples: " + validCounterexamples + "\n");
        header = header.concat("Number of Invalid Counterexamples: " + invalidCounterexamples + "\n");
        header = header.concat("Biggest number of states in a counterexample: " + biggestCounterexample + "\n");
        header = header.concat("Smallest number of states in a counterexample: " + smallestCounterexample + "\n");
        header = header.concat("Number of states used by valid counterexamples: " + usedStates + "\n");
        header = header.concat("Number of transitions used by valid counterexamples: " + usedtransitions + "\n");

        return header;
    }

    public String getData () {
        String separator = " ";
        String data = "";
        data = data + states + separator; //1
        data = data + events + separator; //2
        data = data + decisions + separator; //3
        data = data + stateTransitions + separator; //4
        data = data + eventTransitions + separator; //5
        data = data + getTotalTransitions() + separator; //6
        data = data + usedStates + separator; //17
        data = data + usedtransitions + separator; //18
        data = data + components + separator; //7
        data = data + getComplexity() + separator; //8
        data = data + properties + separator; //9
        data = data + propertiesCaseOne + separator; //10
        data = data + propertiesCaseTwo + separator; //11
        data = data + propertiesCaseThree + separator; //12
        data = data + totalCounterexamples + separator; //13
        data = data + validCounterexamples + separator; //14
        data = data + invalidCounterexamples + separator; //15
        data = data + biggestCounterexample + separator; //16
        data = data + smallestCounterexample + "\n "; //17
        return data;
    }

    public void reset() {
        states = 0;
        eventTransitions = 0;
        stateTransitions = 0;
        events = 0;
        decisions = 0;
        components = 0;
        properties = 0;
        propertiesCaseOne = 0;
        propertiesCaseTwo = 0;
        propertiesCaseThree = 0;
        totalCounterexamples = 0;
        validCounterexamples = 0;
        invalidCounterexamples = 0;
        biggestCounterexample = 0;
        smallestCounterexample = 0;
        usedStates = 0;
        usedtransitions = 0;
    }
}
