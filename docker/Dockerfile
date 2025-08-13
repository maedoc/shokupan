FROM debian:bullseye-slim

# Install Dovecot and required packages
RUN apt-get update && apt-get install -y \
    dovecot-core \
    dovecot-imapd \
    dovecot-pop3d \
    dovecot-lmtpd \
    dovecot-managesieved \
    openssl \
    && rm -rf /var/lib/apt/lists/*

# Create mail directory
RUN mkdir -p /var/mail/testuser/{cur,new,tmp}
RUN mkdir -p /var/mail/testuser/.TODO/{cur,new,tmp}
RUN mkdir -p /var/mail/testuser/.DOING/{cur,new,tmp}
RUN mkdir -p /var/mail/testuser/.DONE/{cur,new,tmp}
RUN mkdir -p /var/mail/testuser/.BACKLOG/{cur,new,tmp}

# Create test user
RUN useradd -m -s /bin/bash testuser
RUN chown -R testuser:testuser /var/mail/testuser

# Copy configuration
COPY dovecot.conf /etc/dovecot/dovecot.conf
COPY 10-auth.conf /etc/dovecot/conf.d/10-auth.conf
COPY 10-mail.conf /etc/dovecot/conf.d/10-mail.conf
COPY 10-master.conf /etc/dovecot/conf.d/10-master.conf
COPY 10-ssl.conf /etc/dovecot/conf.d/10-ssl.conf

# Copy test emails
COPY test-emails/ /var/mail/testuser/
RUN chown -R testuser:testuser /var/mail/testuser

# Generate SSL certificate
RUN openssl req -new -x509 -days 365 -nodes -out /etc/ssl/certs/dovecot.pem -keyout /etc/ssl/private/dovecot.pem -subj "/C=US/ST=Test/L=Test/O=Test/CN=localhost"

# Set permissions
RUN chmod 600 /etc/ssl/private/dovecot.pem
RUN chmod 644 /etc/ssl/certs/dovecot.pem

EXPOSE 143 993

CMD ["dovecot", "-F"]