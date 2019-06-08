using System;
using System.Collections.Generic;
using System.ComponentModel;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using Shared.Logging;

namespace Shared.Repository.LogstashMessages
{
    public class LogMessage : LogstashMessage
    {
        [JsonConverter(typeof(StringEnumConverter))]
        [DefaultValue(null)]
        public LogLevel LogLevel { get; set; }
        [DefaultValue(null)]
        public object Body { get; set; }
        [DefaultValue(null)]
        public KeyValuePair<string, object>[] AdditionalInformation { get; set; }
        [DefaultValue(null)]
        public DateTime CreatedAt { get; set; }

        public override string MessageContent => "logs";
        public string ExecutingAssemblyInfo => System.Reflection.Assembly.GetExecutingAssembly().ToString();
        public string EntryAssemblyInfo => System.Reflection.Assembly.GetEntryAssembly().ToString();
    }
}