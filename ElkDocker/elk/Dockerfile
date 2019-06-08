FROM sebp/elk
RUN rm -f /etc/logstash/conf.d/*
ADD ./logstash.conf /etc/logstash/conf.d/logstash.conf
RUN chmod 777 /etc/logstash/conf.d/logstash.conf
