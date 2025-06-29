FROM gcc:12

RUN apt-get update && apt-get install -y \
    make \
    gdb \
    openssh-server && \
    mkdir /var/run/sshd

# Set the root password (change this in production)
RUN echo 'root:password' | chpasswd

# Allow root login via SSH
RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

EXPOSE 22

# Copy the entrypoint script
COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# Set the entrypoint
ENTRYPOINT ["./entrypoint.sh"]