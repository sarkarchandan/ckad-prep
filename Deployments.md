# Deployments

We need to understand about ReplicaSets before we talk about the Deployments. ReplicaSets are the fundamental K8s object, which maintains the creation and managing the Pods. A Deployment manages the Pods with the help of ReplicaSets. In the evolution story of K8s ReplicaSets came before Deployments. One of the basic promises of K8s is to preserve the desired cluster state. The applications are implemented and running inside the containers and they are hosted by the Pods. Hence, in some sense at any given point in time the running Pods and their number inside the cluster represent the cluster state. Deployment and ReplicaSets are the mechanism, with which this is state is maintained.

Deployments and ReplicaSets implement the self-healing mechanism of K8s. At any given point they maintain the desired number of running Pods. We'd see, that they also depend upon the Pod template. Following are some key points to keep in mind, before we start going into practical handling of Deployments.

- Deployments can scale Pods by adapting the ReplicaSets.
- Deployments and ReplicaSets support zero-downtime deployment of applications i.e., in order to rollout a new cluster state we don't need to take down the application.
- They also enable the quicker rollback utility.
- Previously, we used some `labels` for the Pods but did not make use of them anywhere. We'd shortly see, that the Deployments and ReplicaSets make use of them to manage Pods.

Following is an example YAML specification for a Deployment object.

```yaml
apiVersion: apps/v1 # Specify the API version, this time it is a bit different from Pod
kind: Deployment # Kind of the object is Deployment
metadata: # Place to put useful information about the Deployment like its name and any labels
  name: nginx-deployment # Name of the deployment
  labels: # Label specification for the Deployment. If we have multiple Deployments, we can select a particular one with Deployment. Later we'd see, where this is helpful
    app: my-nginx
    tier: frontend
spec: # Deployment specification
  selector: # Pod selector (encapsulates the mechanism to select Pods that it must manage)
    matchLabels: # We specify, that we want to select Pods, which match the specified labels
      app: nginx
  replicas: 2 # Tells deployment to run 2 pods matching the template (in our case template the is matchLabels defined under the selector)
  template: # Template used to create Pods
    metadata: # Place the useful information about the Pod like specify its labels, based on which it would be selected by the Deployment
      labels: # Labels specification
        app: nginx # Label based on which this Pod would be identified by the Deployment
    spec: # Pod specification
      containers: # Container specification
        - name: nginx # Container name
          image: nginx:1.14.2 # Base image for the container
          ports: # Port specification
            - containerPort: 80 # Container pot
```

The example above is one, in which we have included the Pod template inside the Deployment. That may not be the desirable way to go always. If we want, we could also define Pod templates in separate files and mention them under template. But this style of putting it together is also popular to have a full view of the desired cluster state. In a Deployment specification, following fields are expected.

- `apiVersion` - Which version of the K8s API to use.
- `kind` - What kind of object we want to create.
- `metadata` - Data that helps uniquely identify the object, including a name string, UID, and optional namespace.
- `spec` - What state we desire for the object and that refers to the first level spec, that we see in our example above.

Following are some of the commands, which we'd make use of in he following practice exercises.

```bash
$ k create -f deployment_spec.yaml --save-config
$ k apply -f deployment_spec.yaml
$ k get deployment --show-labels
$ k get deployment -l app=nginx # -l flag is used to filter deployments with label e.g., app:my-nginx
$ k delete deployment <deployment_name>
$ k delete deployment -f deployment_spec.yaml
$ k scale deployment [<deployment_name> | -f deployment_spec.yaml] --replicas=5 # We could also do this by simply updating the YAML specification and use the apply command
```
