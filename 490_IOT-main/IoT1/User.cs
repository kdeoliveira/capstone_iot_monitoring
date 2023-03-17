namespace IoT1
{
    public class User
    {
        public string FirstName { get; set; }
        public string LastName { get; set; }
        public int AgentID { get; set; }
        public int StationNumber { get; set; }
        public string FullName
        {
            get { return FirstName + " " + LastName; }
        }
    }
}
