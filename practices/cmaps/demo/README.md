# README

1. Delete any currently active ConfigMap which may create a conflict.
2. Build Image with the Dockerfile and server.js and name it node-config: `docker image build -t node-configmap .`.
3. Create ConfigMap `app-settings` using the flag `--from-env-file=settings.config`.
4. Create Deployment using node.deployment.yaml file.
5. PortForward to access the application running in port 9000 internally.